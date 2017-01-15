#pragma once

#include "network/HostAddress.h"

#include <vector>

KA_NAMESPACE_BEGIN

class Room;
class User;

class Server
{
public:
	Server();
	~Server();

	bool listen(const HostAddress &ip, ushort port);
	void close();

	std::vector<Room *> rooms() const;
	Room *createRoom(uint id);
	Room *findRoom(uint id);

	User *next();

private:
	BA_DECLARE_PRIVATE
};

KA_NAMESPACE_END
