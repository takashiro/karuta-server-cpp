/***********************************************************************
KARUTA server
Copyright (C) 2016-2017  Kazuichi Takashiro

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

takashiro@qq.com
************************************************************************/

#include "Room.h"
#include "User.h"

#include "util/Json.h"
#include "util/Semaphore.h"

#include <vector>
#include <thread>
#include <mutex>
#include <memory>

KA_NAMESPACE_BEGIN

struct Room::Private
{
	uint id;
	std::vector<User *> users;

	Private()
		: id(0)
	{
	}
};

Room::Room(uint id)
	: d(new Private)
{
	d->id = id;
}

Room::~Room()
{
	delete d;
}

const std::vector<User *> &Room::users() const
{
	return d->users;
}

void Room::addUser(User *user)
{
	d->users.push_back(user);
}

void Room::removeUser(User *user)
{
	for (auto iter = d->users.begin(); iter != d->users.end(); iter++) {
		if (*iter == user) {
			d->users.erase(iter);
			break;
		}
	}
}

void Room::broadcastNotification(int command, const Json &arguments)
{
	for (User *user : d->users) {
		user->notify(command, arguments);
	}
}

void Room::broadcastRequest(int command, const Json &arguments, int timeout)
{
	for (User *user : d->users) {
		user->prepareRequest(command, arguments, timeout);
	}

	broadcastRequest(d->users, timeout);
}

void Room::broadcastRequest(int timeout)
{
	broadcastRequest(d->users, timeout);
}

void Room::broadcastRequest(const std::vector<User *> &users, int timeout)
{
	for (User *user : users) {
		user->executeRequest();
	}
}

User *Room::broadcastRacingRequest(int command, const Json &arguments, int timeout)
{
	for (User *user : d->users) {
		user->prepareRequest(command, arguments, timeout);
	}

	return broadcastRacingRequest(d->users, timeout);
}

User *Room::broadcastRacingRequest(int timeout)
{
	return broadcastRacingRequest(d->users, timeout);
}

User *Room::broadcastRacingRequest(const std::vector<User *> &users, int timeout)
{
	std::shared_ptr<bool> requesting = std::make_shared<bool>(true);
	std::shared_ptr<std::mutex> requestMutex = std::make_shared<std::mutex>();

	User *winner = nullptr;
	Semaphore replied;

	for (User *user : d->users) {
		user->executeRequest([requestMutex,requesting,user,&winner,&replied](const Json &) {
			if (requestMutex->try_lock()) {
				if (*requesting && winner == nullptr) {
					winner = user;
					replied.release();
				}
				requestMutex->unlock();
			}
		});
	}

	if (timeout > 0) {
		if (replied.acquire(1, timeout)) {
			std::unique_lock<std::mutex> lock(*requestMutex);
			*requesting = false;
			return winner;
		}
	} else {
		replied.acquire();
		std::unique_lock<std::mutex> lock(*requestMutex);
		requesting = false;
		return winner;
	}

	std::unique_lock<std::mutex> lock(*requestMutex);
	*requesting = false;
	return nullptr;
}

KA_NAMESPACE_END
