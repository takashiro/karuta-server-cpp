#include "Server.h"
#include "Room.h"

#include <map>

BA_NAMESPACE_BEGIN

struct Server::Private
{
	std::map<uint, Room *> rooms;
};

Server::Server()
	: d(new Private)
{

}

Server::~Server()
{
	for (const std::pair<uint, Room *> &p : d->rooms) {
		delete p.second;
	}

	delete d;
}

BA_NAMESPACE_END
