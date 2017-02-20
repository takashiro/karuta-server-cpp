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

#include "Packet.h"
#include "Json.h"

KA_NAMESPACE_BEGIN

namespace
{
	void writeint(std::ostream &out, int value)
	{
		if (-126 <= value && value <= 127) {
			char ch = static_cast<char>(value);
			out.write(&ch, 1);
		} else {
			int16 num = static_cast<int16>(value);
			if(num == value){
				char ch = '\x81';
				const char *bits = reinterpret_cast<const char *>(&value);
				out.write(&ch, 1);
				out.write(bits, 2);
			}else{
				char ch = '\x80';
				const char *bits = reinterpret_cast<const char *>(&value);
				out.write(&ch, 1);
				out.write(bits, 4);
			}
		}
	}

	int readint(std::istream &in)
	{
		int value = in.get();
		if (value == 0x81) {
			char cmd[2];
			in.read(cmd, 2);
			value = *(reinterpret_cast<int16 *>(cmd));
		} else if (value == 0x80) {
			char cmd[4];
			in.read(cmd, 4);
			value = *(reinterpret_cast<int32 *>(cmd));
		}
		return value;
	}
}

Packet::Packet()
	: command(0)
	, timeout(0)
{
}

Packet::Packet(int command)
	: command(command)
	, timeout(0)
{
}

Packet::Packet(int command, const Json &arguments)
	: command(command)
	, arguments(arguments)
	, timeout(0)
{
}

Packet::Packet(int command, Json &&arguments)
	: command(command)
	, arguments(std::move(arguments))
	, timeout(0)
{
}

std::istream &operator >> (std::istream &in, Packet &packet)
{
	packet.command = readint(in);
	packet.timeout = readint(in);
	in >> packet.arguments;
	return in;
}

std::ostream &operator << (std::ostream &out, const Packet &packet)
{
	writeint(out, packet.command);
	writeint(out, packet.timeout);
	out << packet.arguments;
	return out;
}

KA_NAMESPACE_END
