#include "User.h"
#include "Packet.h"
#include "util/Json.h"
#include "network/WebSocket.h"

#include <map>

KA_NAMESPACE_BEGIN

struct User::Private
{
	WebSocket *socket;

	const std::map<int, User::Action> *actions;

	Private()
		: socket(nullptr)
		, actions(nullptr)
	{
	}

	~Private()
	{
		if (socket) {
			socket->close();
			delete socket;
		}
	}

	static void Listener(User *user)
	{
		User::Private *d = user->d;
		for (;;) {
			std::string message = d->socket->read();
			if (message.empty()) {
				break;
			}

			std::stringstream ss(message);
			Packet packet;
			ss >> packet;

			if (d->actions == nullptr) {
				break;
			}
			auto i = d->actions->find(packet.command);
			if (i != d->actions->end()) {
				User::Action behavior = i->second;
				behavior(user, packet.arguments);
			}
		}
	}
};

User::User(WebSocket *socket)
	: d(new Private)
{
	d->socket = socket;
}


User::~User()
{
	delete d;
}

void User::setAction(const std::map<int, User::Action> *behaviors)
{
	d->actions = behaviors;
}

void User::exec()
{
	Private::Listener(this);
}

void User::notify(int command)
{
	std::stringstream message;
	message << Packet(command);
	d->socket->write(message.str());
}

void User::notify(int command, const Json &arguments)
{
	std::stringstream message;
	message << Packet(command, arguments);
	d->socket->write(message.str());
}

Json User::request(int command, const Json &arguments, int timeout)
{
	Packet packet(command, arguments);
	packet.timeout = timeout;
	std::stringstream message;
	message << packet;
	d->socket->write(message.str());
	return Json();
}

void User::reply(int command, const Json &arguments)
{
	std::stringstream message;
	message << Packet(command, arguments);
	d->socket->write(message.str());
}

KA_NAMESPACE_END
