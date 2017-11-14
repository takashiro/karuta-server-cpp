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

#include "Application.h"

#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <list>

#include <string.h>

#include "EventLoop.h"
#include "Room.h"
#include "Server.h"
#include "User.h"
#include "UserAction.h"

#include <list>

#ifdef KA_OS_WIN
#include <Winsock2.h>
#endif // KA_OS_WIN

#ifdef KA_OS_LINUX
#include <signal.h>
#endif // KA_OS_LINUX

KA_NAMESPACE_BEGIN

#ifdef KA_OS_LINUX
static std::list<Application *> AppList;

static void on_sigterm(int sig)
{
	for (Application *app : AppList) {
		app->quit();
	}
}
#endif

struct Application::Private
{
	Server server;
	int maxUserNum;
	ushort serverPort;
	bool isInteractive;
	EventLoop loop;

	Private()
		: maxUserNum(2)
		, serverPort(2610)
		, isInteractive(true)
	{
	}
};

Application::Application(int argc, const char *argv[])
	: d(new Private)
{
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--max-user-num") == 0) {
			if (i + 1 < argc) {
				i++;
				std::stringstream ss;
				ss << argv[i];
				ss >> d->maxUserNum;
			}
		} else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
			if (i + 1 < argc) {
				i++;
				std::stringstream ss;
				ss << argv[i];
				ss >> d->serverPort;
			}
		} else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--service") == 0) {
			d->isInteractive = false;
		}
	}

#ifdef KA_OS_WIN
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 0);
	WSAStartup(sockVersion, &wsaData);
#endif // KA_OS_WIN

#ifdef KA_OS_LINUX
	AppList.push_front(this);
#endif // KA_OS_LINUX
}

Application::~Application()
{
#ifdef KA_OS_WIN
	WSACleanup();
#endif // KA_OS_WIN
	delete d;
}

int Application::exec()
{
	if (d->server.listen(HostAddress::Any, d->serverPort)) {
		std::cout << "Listening " << d->serverPort << "..." << std::endl;
	} else {
		std::cerr << "Failed to listen to port " << d->serverPort << std::endl;
		return 1;
	}

	std::thread loop_thread([this]() {
		d->loop.exec();
	});

	std::list<std::thread> listeners;

	std::thread server_daemon([&]() {
		for (;;) {
			User *user = d->server.next();
			if (user == nullptr) {
				break;
			}

			//TO-DO: Login check

			listeners.push_back(std::thread([user]() {
				user->setAction(&BasicActions());
				user->exec();
			}));
		}
	});

	if (d->isInteractive) {
		std::string command;
		while (std::cin >> command) {
			//TO-DO: Execute command
		}

		quit();
	} else {
		#ifdef KA_OS_LINUX
		signal(SIGTERM, on_sigterm);
		#endif
	}

	server_daemon.join();
	for (std::thread &listener : listeners) {
		listener.join();
	}
	loop_thread.join();
	return 0;
}

void Application::quit()
{
	d->server.close();
	d->loop.terminate();
}

KA_NAMESPACE_END
