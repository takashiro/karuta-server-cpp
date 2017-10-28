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

#include "Server.h"
#include "Room.h"
#include "User.h"

#include "TcpServer.h"
#include "WebSocket.h"

#include <map>

KA_NAMESPACE_BEGIN

struct Server::Private
{
	std::map<uint, Room *> rooms;
	std::map<uint, User *> users;
	TcpServer socket;
};

Server::Server()
	: d(new Private)
{
}

Server::~Server()
{
	for (const std::pair<uint, Room *> &p : d->rooms) {
		delete p.second;
	}

	delete d;
}

bool Server::listen(const HostAddress &ip, ushort port)
{
	return d->socket.listen(ip, port);
}

void Server::close()
{
	d->socket.close();
}

std::vector<Room *> Server::rooms() const
{
	std::vector<Room *> list;
	list.reserve(d->rooms.size());
	for (const std::pair<uint, Room *> &p : d->rooms) {
		list.push_back(p.second);
	}
	return list;
}

Room *Server::createRoom(uint id)
{
	Room *room = findRoom(id);
	if (room) {
		return room;
	}
	room = new Room(id);
	d->rooms[id] = room;

	room->onAbandon([this, id] () {
		d->rooms.erase(id);
	});

	return room;
}

Room *Server::createRoom(uint id, const std::string &driver)
{
	Room *room = findRoom(id);
	if (room) {
		return room;
	}
	room = new Room(id);
	if (room && room->loadDriver(driver)) {
		d->rooms[id] = room;

		room->onAbandon([this, id] () {
			d->rooms.erase(id);
		});

		return room;
	} else {
		delete room;
		return nullptr;
	}
}

Room *Server::findRoom(uint id)
{
	std::map<uint, Room *>::iterator iter = d->rooms.find(id);
	if (iter != d->rooms.end()) {
		return iter->second;
	} else {
		return nullptr;
	}
}

User *Server::next()
{
	TcpSocket *socket = d->socket.next();
	if (socket) {
		WebSocket *websocket = new WebSocket(socket);
		User *user = new User(websocket);
		user->setServer(this);
		return user;
	} else {
		return nullptr;
	}
}

void Server::addUser(User *user)
{
	if (user->id() > 0) {
		d->users[user->id()] = user;
	}
}

void Server::removeUser(uint id)
{
	auto iter = d->users.find(id);
	if (iter != d->users.end()) {
		d->users.erase(iter);
	}
}

User *Server::findUser(uint id)
{
	auto iter = d->users.find(id);
	if (iter != d->users.end()) {
		return iter->second;
	} else {
		return nullptr;
	}
}

KA_NAMESPACE_END
