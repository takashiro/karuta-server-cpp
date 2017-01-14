#include "User.h"
#include "util/Json.h"
#include "network/WebSocket.h"

#include <sstream>
#include <map>

BA_NAMESPACE_BEGIN

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
		socket->close();
	}

	static void Listener(User *user)
	{
		User::Private *d = user->d;
		for (;;) {
			std::string message = d->socket->read();
			std::stringstream ss(message);
			int command = ss.get();
			if (command <= 0) {
				break;
			}

			if (d->actions == nullptr) {
				break;
			}
			auto i = d->actions->find(command);
			if (i != d->actions->end()) {
				User::Action behavior = i->second;
				Json arguments;
				ss >> arguments;
				behavior(user, arguments);
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
	Json null;
	notify(command, null);
}

void User::notify(int command, const Json &arguments)
{
	std::stringstream message;
	if (command < 255) {
		char ch = static_cast<char>(command);
		message.write(&ch, 1);
	} else {
		char ch[5];
		ch[0] = '\255';
		const char *command_bits = reinterpret_cast<const char *>(&command);
		for (int i = 1; i <= 4; i++) {
			ch[i] = command_bits[4 - i];
		}
		message.write(ch, 5);
	}
	message << arguments;
	d->socket->write(message.str());
}

BA_NAMESPACE_END
