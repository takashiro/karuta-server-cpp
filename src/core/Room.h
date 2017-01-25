#pragma once

#include "global.h"

#include <vector>

KA_NAMESPACE_BEGIN

class Json;
class User;

class Room
{
public:
	Room(uint id);
	~Room();

	const std::vector<User *> &users() const;
	void addUser(User *user);
	void removeUser(User *user);

	void broadcastNotification(int command, const Json &arguments);
	User *broadcastRacingRequest(int command, const Json &arguments, int timeout = 0);
	User *broadcastRacingRequest(int timeout = 0);
	User *broadcastRacingRequest(const std::vector<User *> &users, int timeout = 0);

private:
	KA_DECLARE_PRIVATE
};

KA_NAMESPACE_END
