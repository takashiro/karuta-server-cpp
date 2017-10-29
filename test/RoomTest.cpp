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

#include "test.h"

#include <thread>

#include <Room.h>
#include <Server.h>
#include <User.h>
#include <Json.h>

namespace UnitTest
{
	TEST_CLASS(RoomUnit)
	{
	public:

		TEST_METHOD(RacingRequestTest)
		{
			const int ACTION_ID = 1001;
			ushort port = static_cast<ushort>(rand(1000, 0xFFFF));
			Server server;
			assert(server.listen(HostAddress::Any, port));

			int winner_answer = 0;
			std::thread server_thread([&]() {
				std::vector<std::thread> listeners;
				Room room(0);
				for (int i = 0; i < 5; i++) {
					User *user = server.next();
					room.addUser(user);
					std::thread thread([=]() {
						user->exec();
					});
					listeners.push_back(std::move(thread));
				}

				const std::vector<User *> &users = room.users();
				for (int i = 0; i < 5; i++) {
					JsonArray args;
					args.push_back(i);
					args.push_back(i);
					users[i]->prepareRequest(ACTION_ID, args, 1);
				}

				User *winner = room.broadcastRacingRequest(5);
				if (winner) {
					Json reply = winner->getReply();
					winner_answer = reply.toInt();
				}

				for (User *user : users) {
					user->disconnect();
				}

				for (std::thread &thread : listeners) {
					thread.join();
				}
			});

			std::map<int, User::Action> actions;
			actions[ACTION_ID] = [ACTION_ID](User *user, const Json &arguments) {
				int wait = arguments[0].toInt();
				int num = arguments[1].toInt();
				if (wait > 0) {
					std::this_thread::sleep_for(std::chrono::milliseconds(wait * 100));
				}
				user->reply(ACTION_ID, num + 1);
			};

			std::vector<std::thread> user_threads;
			User users[5];
			for (User &user : users) {
				user.setAction(&actions);

				std::thread thread([&]() {
					user.connect(HostAddress::Local, port);
					user.exec();
				});
				user_threads.push_back(std::move(thread));
			}

			server_thread.join();
			for (std::thread &thread : user_threads) {
				thread.join();
			}

			assert(winner_answer == 1);
		}


		TEST_METHOD(ParellelRequestTest)
		{
			ushort port = static_cast<ushort>(rand(1000, 0xFFFF));
			Server server;
			assert(server.listen(HostAddress::Any, port));

			int questions[5] = {rand(), rand(), rand(), rand(), rand()};
			int answers[5];
			std::thread server_thread([&]() {
				Room room(0);
				std::vector<std::thread> listeners;
				for (int i = 0; i < 5; i++) {
					User *user = server.next();
					room.addUser(user);
					user->prepareRequest(11, questions[i], 5);
					std::thread listener([=]() {
						user->exec();
					});
					listeners.push_back(std::move(listener));
				}

				const std::vector<User *> &users = room.users();
				room.broadcastRequest(users);
				for (int i = 0; i < 5; i++) {
					User *user = users[i];
					Json reply = user->getReply();
					answers[i] = reply.toInt();
					user->disconnect();
				}

				for (std::thread &listener : listeners) {
					listener.join();
				}
			});

			std::map<int, User::Action> actions;
			actions[11] = [&](User *user, const Json &arguments) {
				int num = arguments.toInt();
				user->reply(11, num + 1010);
			};

			User users[5];
			std::vector<std::thread> clients;
			for (User &user : users) {
				user.connect(HostAddress::Local, port);
				user.setAction(&actions);
				std::thread client([&]() {
					user.exec();
				});
				clients.push_back(std::move(client));
			}

			server_thread.join();
			for (std::thread &client : clients) {
				client.join();
			}

			for (int i = 0; i < 5; i++) {
				assert(questions[i] + 1010 == answers[i]);
			}
		}
	};
}
