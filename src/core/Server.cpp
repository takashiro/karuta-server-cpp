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
	Room *lobby;
	std::map<uint, Room *> rooms;
	TcpServer socket;
};

Server::Server()
	: d(new Private)
{
	d->lobby = new Room(0);
}

Server::~Server()
{
	for (const std::pair<uint, Room *> &p : d->rooms) {
		delete p.second;
	}

	delete d->lobby;
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

	room->bind(Room::abandoned, [this, id] () {
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

		room->bind(Room::abandoned, [this, id] () {
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

		user->bind(User::disconnected, [this, user] () {
			Room *room = user->room();
			if (room) {
				room->removeUser(user);
			}
		});

		return user;
	} else {
		return nullptr;
	}
}

void Server::addUser(User *user)
{
	if (user->id() > 0) {
		d->lobby->addUser(user);
	}
}

void Server::removeUser(User *user)
{
	d->lobby->removeUser(user);
}

User *Server::findUser(uint id)
{
	return d->lobby->findUser(id);
}

KA_NAMESPACE_END
