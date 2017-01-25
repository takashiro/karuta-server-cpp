#include "User.h"
#include "Packet.h"
#include "util/Json.h"
#include "util/Semaphore.h"
#include "network/WebSocket.h"

#include <map>

KA_NAMESPACE_BEGIN

struct User::Private
{
	WebSocket *socket;
	const std::map<int, User::Action> *actions;

	int replyCommand;
	Semaphore replySem;
	Json reply;

	int requestTimeout;
	std::string requestMessage;
	std::function<void(const Json &)> replyCallback;

	Private()
		: socket(nullptr)
		, actions(nullptr)
		, replySem(0)
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
			if (packet.command == 0) {
				d->socket->close();
				break;
			}

			if (d->replyCommand > 0 && d->replyCommand == packet.command) {
				d->reply = std::move(packet.arguments);
				d->replySem.release();
				if (d->replyCallback) {
					d->replyCallback(d->reply);
				}
				continue;
			}

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

User::User()
	: d(new Private)
{
}

User::User(WebSocket *socket)
	: d(new Private)
{
	d->socket = socket;
}


User::~User()
{
	delete d;
}

bool User::connect(const HostAddress &ip, ushort port)
{
	if (d->socket) {
		return false;
	}

	WebSocket *socket = new WebSocket;
	if (socket->open(ip, port)) {
		d->socket = socket;
		return true;
	} else {
		delete socket;
		return false;
	}
}

void User::setAction(const std::map<int, User::Action> *behaviors)
{
	d->actions = behaviors;
}

void User::exec()
{
	Private::Listener(this);
}

void User::disconnect()
{
	notify(0, 1);
	d->socket->close();
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
	d->replyCommand = command;
	d->socket->write(message.str());

	if (timeout > 0) {
		if (d->replySem.acquire(1, timeout)) {
			return d->reply;
		} else {
			return Json();
		}
	} else {
		d->replySem.acquire();
		return d->reply;
	}
}

void User::reply(int command, const Json &arguments)
{
	std::stringstream message;
	message << Packet(command, arguments);
	d->socket->write(message.str());
}

void User::prepareRequest(int command, const Json &arguments, int timeout)
{
	Packet packet;
	packet.command = command;
	packet.arguments = arguments;
	packet.timeout = timeout;

	std::stringstream ss;
	ss << packet;
	d->requestMessage = ss.str();
	d->requestTimeout = timeout;
	d->replyCommand = command;
}

bool User::executeRequest(const std::function<void(const Json &)> &callback)
{
	d->replyCallback = callback;
	d->socket->write(d->requestMessage);
	if (d->requestTimeout > 0) {
		return d->replySem.acquire(1, d->requestTimeout);
	} else {
		d->replySem.acquire();
		return true;
	}
}

bool User::executeRequest(std::function<void(const Json &)> &&callback)
{
	d->replyCallback = std::move(callback);
	d->socket->write(d->requestMessage);
	if (d->requestTimeout > 0) {
		return d->replySem.acquire(1, d->requestTimeout);
	} else {
		d->replySem.acquire();
		return true;
	}
}

Json User::getReply() const
{
	return d->reply;
}

KA_NAMESPACE_END
