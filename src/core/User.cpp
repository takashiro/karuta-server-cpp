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

#include "User.h"
#include "Packet.h"
#include "Json.h"
#include "Semaphore.h"
#include "WebSocket.h"

#include <map>
#include <sstream>

KA_NAMESPACE_BEGIN

struct User::Private
{
	WebSocket *socket;
	const std::map<int, UserAction> *actions;

	int replyCommand;
	Semaphore replySem;
	Json reply;

	int requestTimeout;
	std::string requestMessage;
	std::function<void(const Json &)> replyCallback;

	uint id;
	Room *room;
	Server *server;

	Private()
		: socket(nullptr)
		, actions(nullptr)
		, replySem(0)
		, id(0)
		, room(nullptr)
		, server(nullptr)
	{
	}

	~Private()
	{
		if (socket) {
			socket->close();
			delete socket;
		}
	}

	bool executeRequest()
	{
		if (requestTimeout > 0) {
			return replySem.acquire(1, requestTimeout);
		} else {
			replySem.acquire();
			return true;
		}
	}
};

User::User()
	: d(new Private)
{
}

User::User(WebSocket *socket)
	: d(new Private)
{
	d->socket = socket;
}


User::~User()
{
	delete d;
}

bool User::connect(const HostAddress &ip, ushort port)
{
	if (d->socket) {
		return false;
	}

	WebSocket *socket = new WebSocket;
	if (socket->open(ip, port)) {
		d->socket = socket;
		return true;
	} else {
		delete socket;
		return false;
	}
}

void User::setAction(const std::map<int, UserAction> *actions)
{
	d->actions = actions;
}

void User::exec()
{
	if (d->socket == nullptr) {
		return;
	}

	std::string message;
	for (;;) {
		if (!d->socket->read(message)) {
			break;
		}

		std::stringstream ss(message);
		Packet packet;
		ss >> packet;
		if (packet.command == 0) {
			d->socket->close();
			break;
		}

		if (d->replyCommand > 0 && d->replyCommand == packet.command) {
			d->reply = std::move(packet.arguments);
			d->replySem.release();
			if (d->replyCallback) {
				d->replyCallback(d->reply);
			}
			continue;
		}

		if (d->actions == nullptr) {
			break;
		}
		auto i = d->actions->find(packet.command);
		if (i != d->actions->end()) {
			UserAction behavior = i->second;
			behavior(this, packet.arguments);
		}
	}
}

void User::disconnect()
{
	if (d->socket) {
		d->socket->close();
	}
}

void User::notify(int command)
{
	if (d->socket) {
		std::stringstream message;
		message << Packet(command);
		d->socket->write(message.str());
	}
}

void User::notify(int command, const Json &arguments)
{
	if (d->socket) {
		std::stringstream message;
		message << Packet(command, arguments);
		d->socket->write(message.str());
	}
}

Json User::request(int command, const Json &arguments, int timeout)
{
	if (d->socket == nullptr) {
		return Json();
	}

	Packet packet(command, arguments);
	packet.timeout = timeout;
	std::stringstream message;
	message << packet;
	d->replyCommand = command;
	d->socket->write(message.str());

	if (timeout > 0) {
		if (d->replySem.acquire(1, timeout)) {
			return d->reply;
		} else {
			return Json();
		}
	} else {
		d->replySem.acquire();
		return d->reply;
	}
}

void User::reply(int command, const Json &arguments)
{
	if (d->socket) {
		std::stringstream message;
		message << Packet(command, arguments);
		d->socket->write(message.str());
	}
}

void User::prepareRequest(int command, const Json &arguments, int timeout)
{
	Packet packet;
	packet.command = command;
	packet.arguments = arguments;
	packet.timeout = timeout;

	std::stringstream ss;
	ss << packet;
	d->requestMessage = ss.str();
	d->requestTimeout = timeout;
	d->replyCommand = command;
}

bool User::executeRequest(const std::function<void(const Json &)> &callback)
{
	if (d->socket) {
		d->replyCallback = callback;
		d->socket->write(d->requestMessage);
		return d->executeRequest();
	} else {
		return false;
	}
}

bool User::executeRequest(std::function<void(const Json &)> &&callback)
{
	if (d->socket) {
		d->replyCallback = std::move(callback);
		d->socket->write(d->requestMessage);
		return d->executeRequest();
	} else {
		return false;
	}
}

bool User::executeRequest()
{
	if (d->socket) {
		d->replyCallback = nullptr;
		d->socket->write(d->requestMessage);
		return d->executeRequest();
	} else {
		return false;
	}
}

Json User::getReply() const
{
	return d->reply;
}

uint User::id() const
{
	return d->id;
}

void User::setId(uint id)
{
	d->id = id;
}

bool User::isLoggedIn() const
{
	return d->id > 0;
}

Room *User::room() const
{
	return d->room;
}

void User::setRoom(Room *room)
{
	d->room = room;
}

Server *User::server() const
{
	return d->server;
}

void User::setServer(Server *server)
{
	d->server = server;
}

KA_NAMESPACE_END
