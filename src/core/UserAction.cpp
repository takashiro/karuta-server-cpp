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

#include "GameDriver.h"
#include "Json.h"
#include "protocol.h"
#include "Room.h"
#include "Server.h"
#include "Semaphore.h"
#include "User.h"

#include <fstream>

KA_NAMESPACE_BEGIN

static std::map<int, UserAction> CreateActions()
{
	static std::map<int, UserAction> actions;

	actions[net::CheckVersion] = [] (User *user, const Json &args) {
		int client_version = args.toInt();
		// Check Version
	};

	actions[net::RequestRoomId] = [] (User *user, const Json &) {
		static uint nextRoomId = 0;
		static Semaphore sem(1);

		Server *server = user->server();
		int id = 0;

		sem.acquire();
		nextRoomId++;
		for (int i = 0; i < 5; i++) {
			if (server->findRoom(nextRoomId) != nullptr) {
				nextRoomId++;
			} else {
				id = nextRoomId;
				break;
			}
		}
		sem.release();

		user->notify(net::RequestRoomId, id);
	};

	actions[net::RequestUserId] = [] (User *user, const Json &) {
		static uint nextUserId = 0;
		static Semaphore sem(1);

		Server *server = user->server();
		int id = 0;

		sem.acquire();
		nextUserId++;
		for (int i = 0; i < 5; i++) {
			if (server->findUser(nextUserId) != nullptr) {
				nextUserId++;
			} else {
				id = nextUserId;
				break;
			}
		}
		sem.release();

		user->notify(net::RequestUserId, id);
	};

	actions[net::Login] = [] (User *user, const Json &args) {
		uint uid = 0;
		if (args.isObject() && args.contains("uid")) {
			uid = args["uid"].toInt();
			if (uid > 0) {
				//TO-DO: verify login credentials
				user->setId(uid);

				Server *server = user->server();
				User *existed = server->findUser(uid);
				if (existed && existed != user) {
					existed->disconnect();
				}
				server->addUser(user);
			}
		}
		user->notify(net::Login, uid);
	};

	actions[net::Logout] = [] (User *user, const Json &args) {
		user->notify(net::Logout);
		user->disconnect();
	};

	actions[net::CreateRoom] = [] (User *user, const Json &args) {
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
			GameDriver *driver = new_room->driver();
			if (driver) {
				user->setExtraAction(driver->actions());
			}
			if (new_room->owner() == nullptr) {
				new_room->setOwner(user);
			}
			new_room->addUser(user);
		} else {
			user->notify(net::CreateRoom, 0);
		}
	};

	actions[net::EnterRoom] = [] (User *user, const Json &args) {
		if (!user->isLoggedIn()) {
			return;
		}

		Server *server = user->server();
		if (server == nullptr) {
			user->disconnect();
			delete user;
			return;
		}

		if (args.isObject() && args.contains("id")) {
			// Find the room
			uint room_id = args["id"].toUInt();
			Room *new_room = server->findRoom(room_id);
			if (new_room && args.contains("game")) {
				std::string game = args["game"].toString();
				GameDriver *driver = new_room->driver();
				if (driver == nullptr || driver->name() != game) {
					new_room = nullptr;
				}
			}

			if (new_room) {
				// Leave the old room
				Room *old_room = user->room();
				if (old_room) {
					old_room->removeUser(user);
					user->setExtraAction(nullptr);
				}

				// Enter the new room
				GameDriver *driver = new_room->driver();
				if (driver) {
					user->setExtraAction(driver->actions());
					new_room->addUser(user);
					user->notify(net::UpdateRoom, driver->config());
				} else {
					new_room->addUser(user);
				}
			} else {
				// The room is not found
				user->notify(net::EnterRoom);
			}
		} else {
			// Just want to exit the room
			Room *room = user->room();
			if (room) {
				room->removeUser(user);
				server->addUser(user);
			}
		}
	};

	actions[net::UpdateRoom] = [] (User *user, const Json &args) {
		if (!user->isLoggedIn()) {
			return;
		}

		Room *room = user->room();
		if (room == nullptr || room->owner() != user) {
			return;
		}

		GameDriver *driver = room->driver();
		if (driver) {
			driver->setConfig(args);

			const Json &new_config = driver->config();
			room->broadcastNotification(net::UpdateRoom, new_config);
		}
	};

	actions[net::Speak] = [] (User *user, const Json &message) {
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

	actions[net::StartGame] = [] (User *user, const Json &) {
		if (!user->isLoggedIn()) {
			return;
		}

		Room *room = user->room();
		if (room == nullptr || room->owner() != user) {
			return;
		}

		room->setState(Room::State::Starting);
		room->broadcastNotification(net::StartGame);
		GameDriver *driver = room->driver();
		if (driver) {
			const std::vector<User *> &users = room->users();
			for (User *user : users) {
				driver->addPlayer(user);
			}
			driver->start();
		}
	};

	return actions;
}

const std::map<int, UserAction> &BasicActions()
{
	static std::map<int, UserAction> actions = CreateActions();
	return actions;
}

KA_NAMESPACE_END
