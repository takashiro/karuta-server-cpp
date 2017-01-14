#include "test.h"

#include <thread>

#include <core/Server.h>
#include <core/User.h>
#include <util/Json.h>
#include <network/WebSocket.h>

namespace
{
	int result = 0;

	void SetNumber(User *user, const Json &arg)
	{
		result = arg.toInt();
	}
}

namespace UnitTest
{
	TEST_CLASS(ServerTest)
	{
	public:

		TEST_METHOD(ServerCreateRoomTest)
		{
			Server server;
			Room *room1 = server.createRoom(19941010);
			Room *room2 = server.createRoom(19941010);
			assert(room1 == room2);

			std::vector<Room *> rooms = server.rooms();
			assert(rooms.size() == 1);
			assert(rooms[0] == room1);
		}

		TEST_METHOD(ServerFindRoomTest)
		{
			Server server;
			Room *room1 = server.createRoom(19941010);
			Room *room2 = server.findRoom(19941010);
			assert(room1 == room2);

			Room *room3 = server.findRoom(19920526);
			assert(room3 == nullptr);
		}

		TEST_METHOD(ServerNextUserTest)
		{
			ushort port = static_cast<ushort>(rand(1000, 65535));
			int number = rand();

			std::map<int, User::Action> actions;
			actions[1010] = SetNumber;

			Server server;
			assert(server.listen(HostAddress::Any, port));

			std::thread server_thread([&]() {
				User *user = server.next();
				if (user) {
					user->setAction(&actions);
					user->exec();
					delete user;
				}
			});

			std::thread client_thread([&]() {
				WebSocket *socket = new WebSocket;
				if (socket->open(HostAddress::Local, port)) {
					User user(socket);
					user.notify(1010, number);
				} else {
					delete socket;
				}
			});

			server_thread.join();
			client_thread.join();

			assert(result == number);
		}
	};
}
