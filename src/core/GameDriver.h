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

#include "Object.h"
#include "UserAction.h"

#include <string>
#include <map>

KA_NAMESPACE_BEGIN

class Room;
class User;

class KA_DLL_EXPORT GameDriver : public Object
{
public:
	GameDriver();
	virtual ~GameDriver();

	void setName(const std::string &name);
	const std::string &name() const;

	void setRoom(Room *room);
	Room *room();

	void wait();

	virtual void setConfig(const Json &arg) = 0;
	virtual const Json &config() const = 0;

	virtual void start();
	virtual void run() = 0;
	virtual void end() = 0;

	virtual void addPlayer(User *user) = 0;
	virtual void removePlayer(User *user) = 0;

	virtual const std::map<int, UserAction> *actions() const = 0;

	static void started() {}
	static void ended() {}

protected:
	KA_DECLARE_PRIVATE
};

KA_NAMESPACE_END
