#include "test.h"

#include <thread>

#include <core/Room.h>
#include <core/Server.h>
#include <core/User.h>
#include <util/Json.h>

namespace UnitTest
{
	TEST_CLASS(RoomTest)
	{
	public:

		TEST_METHOD(RacingRequestTest)
		{
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
					std::thread thread([&]() {
						user->exec();
					});
					listeners.push_back(std::move(thread));
				}

				const std::vector<User *> &users = room.users();
				for (int i = 0; i < 5; i++) {
					JsonArray args;
					args.push_back(i);
					args.push_back(i);
					users[i]->prepareRequest(10, args, 1);
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
			actions[10] = [](User *user, const Json &arguments) {
				int wait = arguments[0].toInt();
				int num = arguments[1].toInt();
				if (wait > 0) {
					std::this_thread::sleep_for(std::chrono::milliseconds(wait * 100));
				}
				user->reply(10, num + 1);
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
				room.broadcastRequest(users, 5);
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