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

#include "HostAddress.h"

#include <vector>

KA_NAMESPACE_BEGIN

class Room;
class User;

class KA_DLL_EXPORT Server
{
public:
	Server();
	~Server();

	bool listen(const HostAddress &ip, ushort port);
	void close();

	std::vector<Room *> rooms() const;
	Room *createRoom(uint id);
	Room *findRoom(uint id);

	User *next();

private:
	KA_DECLARE_PRIVATE
};

KA_NAMESPACE_END
