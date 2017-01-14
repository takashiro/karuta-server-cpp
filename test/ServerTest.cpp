#include "test.h"

#include <core/Server.h>

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
	};
}
