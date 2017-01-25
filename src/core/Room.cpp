#include "Room.h"
#include "User.h"

#include "util/Json.h"
#include "util/Semaphore.h"

#include <vector>
#include <thread>
#include <mutex>

KA_NAMESPACE_BEGIN

struct Room::Private
{
	uint id;
	std::vector<User *> users;
	bool racingRequestProceeding;
	std::mutex racingRequesMutex;

	Private()
		: id(0)
		, racingRequestProceeding(false)
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
	User *winner = nullptr;
	Semaphore replied;

	d->racingRequestProceeding = true;
	for (User *user : d->users) {
		user->executeRequest([&](const Json &) {
			if (d->racingRequesMutex.try_lock()) {
				if (d->racingRequestProceeding && winner == nullptr) {
					winner = user;
					replied.release();
				}
				d->racingRequesMutex.unlock();
			}
		});
	}

	if (timeout > 0) {
		if (replied.acquire(1, timeout)) {
			std::unique_lock<std::mutex> lock(d->racingRequesMutex);
			d->racingRequestProceeding = false;
			return winner;
		}
	} else {
		replied.acquire();
		std::unique_lock<std::mutex> lock(d->racingRequesMutex);
		d->racingRequestProceeding = false;
		return winner;
	}

	std::unique_lock<std::mutex> lock(d->racingRequesMutex);
	d->racingRequestProceeding = false;
	return nullptr;
}

KA_NAMESPACE_END
