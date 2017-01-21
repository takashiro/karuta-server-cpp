#include "test.h"
#include <core/Packet.h>

namespace UnitTest
{
	TEST_CLASS(PacketTest)
	{
	public:
		
		TEST_METHOD(PacketIOTest)
		{
			int command = rand(0, 0x7FFFFFFF);
			int timeout = rand(0, 0x7FFFFFFF);
			Json arguments(rand());

			Packet in(command, arguments);
			in.timeout = timeout;
			std::stringstream buffer;
			buffer << in;

			Packet out;
			buffer >> out;

			assert(out.command == command);
			assert(out.arguments.toInt() == arguments.toInt());
			assert(out.timeout == timeout);
		}

		TEST_METHOD(InvalidPacketTest)
		{
			char message[6] = {'\0', '\0', 'n', 'u', 'l', 'l'};
			std::stringstream ss;
			ss.write(message, 6);
			Packet packet;
			ss >> packet;
			assert(packet.command == 0);
			assert(packet.timeout == 0);
			assert(packet.arguments.isNull());
		}

	};
}
