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

#include "UserAction.h"

#include "Json.h"
#include "protocol.h"
#include "Room.h"
#include "Server.h"
#include "User.h"

#include <fstream>

KA_NAMESPACE_BEGIN

const std::map<int, UserAction> &BasicActions()
{
	static std::map<int, UserAction> actions;

	if (actions.empty()) {

		actions[net::CheckVersion] = [](User *user, const Json &args) {
			int client_version = args.toInt();
			// Check Version
		};

		actions[net::Login] = [](User *user, const Json &args) {
			if (args.isObject() && args.contains("uid")) {
				uint uid = args["uid"].toInt();
				if (uid > 0) {
					//TO-DO: verify login credentials
					user->setId(uid);
				}
			}
		};

		actions[net::Logout] = [](User *user, const Json &args) {
			Room *room = user->room();
			if (room) {
				room->removeUser(user);
			}
			user->disconnect();
		};

		actions[net::CreateRoom] = [](User *user, const Json &args) {
			Room *old_room = user->room();
			if (old_room) {
				old_room->removeUser(user);
			}

			int room_id = args["id"].toInt();
			std::string room_game = args["game"].toString();

			Server *server = user->server();
			Room *new_room = server->createRoom(room_id, room_game);
			if (new_room) {
				new_room->addUser(user);
			}
		};

		actions[net::EnterRoom] = [](User *user, const Json &args) {
			Room *old_room = user->room();
			if (old_room) {
				old_room->removeUser(user);
			}

			Server *server = user->server();
			Room *new_room = server->findRoom(args.toInt());
			if (new_room) {
				new_room->addUser(user);
			}
		};

		actions[net::Speak] = [](User *user, const Json &message) {
			if (!message.isString()) {
				return;
			}
			
			JsonObject args;
			args["uid"] = static_cast<int>(user->id());
			args["message"] = message;
			
			Room *room = user->room();
			if (room) {
				room->broadcastNotification(net::Speak, args);
			}
		};
	}

	return actions;
}

KA_NAMESPACE_END
