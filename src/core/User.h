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

#include <map>

KA_NAMESPACE_BEGIN

class EventLoop;
class Json;
class Room;
class Semaphore;
class Server;
class WebSocket;

struct HostAddress;

class KA_DLL_EXPORT User : public Object
{
public:
	User();
	User(WebSocket *socket);
	~User();

	bool connect(const HostAddress &ip, ushort port);

	using Action = UserAction;
	void setAction(const std::map<int, Action> *handlers);
	void setExtraAction(const std::map<int, Action> *handlers);
	void exec(EventLoop *loop = nullptr);

	void disconnect();
	static void disconnected() {}

	void notify(int command);
	void notify(int command, const Json &arguments);

	Json request(int command, const Json &arguments, int timeout = 0);
	void reply(int command, const Json &arguments);

	void prepareRequest(int command, const Json &arguments, int timeout = 0);
	void executeRequest(const std::function<void(const Json &)> &callback);
	void executeRequest(std::function<void(const Json &)> &&callback);
	void executeRequest();

	Json waitForReply() const;
	Json getReply() const;

	uint id() const;
	void setId(uint id);

	bool isLoggedIn() const;

	Room *room() const;
	void setRoom(Room *room);

	Server *server() const;
	void setServer(Server *server);

private:
	KA_DECLARE_PRIVATE
};

KA_NAMESPACE_END
