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
