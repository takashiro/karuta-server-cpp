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

static void user_logout(User *user, const Json &args)
{
	Room *room = user->room();
	if (room) {
		room->removeUser(user);
	}
	Server *server = user->server();
	if (server) {
		server->removeUser(user->id());
	}
	user->notify(net::Logout);
	user->disconnect();
};

const std::map<int, UserAction> &BasicActions()
{
	static std::map<int, UserAction> actions;

	if (actions.empty()) {

		actions[net::CheckVersion] = [](User *user, const Json &args) {
			int client_version = args.toInt();
			// Check Version
		};

		actions[net::Login] = [](User *user, const Json &args) {
			uint uid = 0;
			if (args.isObject() && args.contains("uid")) {
				uid = args["uid"].toInt();
				if (uid > 0) {
					//TO-DO: verify login credentials
					user->setId(uid);

					Server *server = user->server();
					User *existed = server->findUser(uid);
					if (existed) {
						user_logout(existed, Json());
					}
					server->addUser(user);
				}
			}
			user->notify(net::Login, uid);
		};

		actions[net::Logout] = user_logout;

		actions[net::CreateRoom] = [](User *user, const Json &args) {
			if (!user->isLoggedIn() || !args.isObject() || !args.contains("id") || !args.contains("game")) {
				return;
			}

			Room *old_room = user->room();
			if (old_room) {
				old_room->removeUser(user);
			}

			int room_id = args["id"].toInt();
			std::string room_game = args["game"].toString();

			Server *server = user->server();
			Room *new_room = server->createRoom(room_id, room_game);
			if (new_room) {
				if (new_room->owner() == nullptr) {
					new_room->setOwner(user);
				}
				new_room->addUser(user);
			}
		};

		actions[net::EnterRoom] = [](User *user, const Json &args) {
			if (!user->isLoggedIn() || !args.isObject() || !args.contains("id") || !args.contains("game")) {
				return;
			}

			Room *old_room = user->room();
			if (old_room) {
				old_room->removeUser(user);
			}

			uint room_id = args["id"].toUInt();
			std::string game = args["game"].toString();

			Server *server = user->server();
			Room *new_room = server->findRoom(room_id);
			if (new_room && new_room->driver() == game) {
				new_room->addUser(user);
			}
		};

		actions[net::Speak] = [](User *user, const Json &message) {
			if (!user->isLoggedIn() || !message.isString()) {
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
