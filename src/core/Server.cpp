#include "Server.h"
#include "Room.h"

#include "network/TcpServer.h"

#include <map>

BA_NAMESPACE_BEGIN

struct Server::Private
{
	std::map<uint, Room *> rooms;
	TcpServer socket;
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

bool Server::listen(const HostAddress & ip, ushort port)
{
	return d->socket.listen(ip, port);
}

void Server::close()
{
	d->socket.close();
}

std::vector<Room *> Server::rooms() const
{
	std::vector<Room *> list;
	list.reserve(d->rooms.size());
	for (const std::pair<uint, Room *> &p : d->rooms) {
		list.push_back(p.second);
	}
	return list;
}

Room *Server::createRoom(uint id)
{
	Room *room = findRoom(id);
	if (room) {
		return room;
	}
	room = new Room(id);
	d->rooms[id] = room;
	return room;
}

Room *Server::findRoom(uint id)
{
	std::map<uint, Room *>::iterator iter = d->rooms.find(id);
	if (iter != d->rooms.end()) {
		return iter->second;
	} else {
		return nullptr;
	}
}

BA_NAMESPACE_END
