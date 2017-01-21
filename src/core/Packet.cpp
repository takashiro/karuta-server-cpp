#include "Packet.h"
#include "util/Json.h"

KA_NAMESPACE_BEGIN

namespace
{
	void writeint(std::ostream &out, int command)
	{
		if (command < 127) {
			char ch = static_cast<char>(command);
			out.write(&ch, 1);
		} else {
			char ch[5];
			ch[0] = 127;
			const char *command_bits = reinterpret_cast<const char *>(&command);
			for (int i = 1; i <= 4; i++) {
				ch[i] = command_bits[i - 1];
			}
			out.write(ch, 5);
		}
	}

	int readint(std::istream &in)
	{
		int value = in.get();
		if (value < 0) {
			return 0;
		} else if (value == 127) {
			char cmd[4];
			in.read(cmd, 4);
			value = *(reinterpret_cast<int *>(cmd));
		}
		return value;
	}
}

Packet::Packet()
	: command(0)
{
}

Packet::Packet(int command)
	: command(command)
{
}

Packet::Packet(int command, const Json &arguments)
	: command(command)
	, arguments(arguments)
{
}

Packet::Packet(int command, Json &&arguments)
	: command(command)
	, arguments(std::move(arguments))
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
