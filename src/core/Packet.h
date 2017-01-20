#pragma once

#include "util/Json.h"

#include <sstream>

KA_NAMESPACE_BEGIN

class Json;

struct Packet
{
	Packet();
	Packet(int command);
	Packet(int command, const Json &arguments);
	Packet(int command, Json &&arguments);

	friend std::istream &operator >> (std::istream &in, Packet &packet);
	friend std::ostream &operator << (std::ostream &out, const Packet &packet);

	int command;
	Json arguments;
	int timeout;
};

KA_NAMESPACE_END
