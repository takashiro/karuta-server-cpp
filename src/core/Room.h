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

#pragma once

#include "global.h"

#include <functional>
#include <vector>
#include <string>

KA_NAMESPACE_BEGIN

class GameDriver;
class Json;
class User;

class KA_DLL_EXPORT Room
{
public:
	Room(uint id);
	~Room();

	Room(Room &&source);
	Room(const Room &room) = delete;

	User *owner() const;
	void setOwner(User *user);

	bool loadDriver(const std::string &driver_name);
	GameDriver *driver() const;

	const std::vector<User *> &users() const;
	void addUser(User *user);
	void removeUser(User *user);
	User *findUser(uint id) const;

	void broadcastNotification(int command);
	void broadcastNotification(int command, const Json &arguments);
	void broadcastNotification(int command, const Json &arguments, User *except);

	void broadcastRequest(int command, const Json &arguments, int timeout = 0);
	void broadcastRequest();
	void broadcastRequest(const std::vector<User *> &users);

	User *broadcastRacingRequest(int command, const Json &arguments, int timeout = 0);
	User *broadcastRacingRequest(int timeout = 0);
	User *broadcastRacingRequest(const std::vector<User *> &users, int timeout = 0);

	void onAbandon(const std::function<void()> &handler);

private:
	KA_DECLARE_PRIVATE
};

KA_NAMESPACE_END
