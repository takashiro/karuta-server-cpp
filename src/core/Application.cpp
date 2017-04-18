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

#include <string.h>

#include "Room.h"
#include "Server.h"
#include "User.h"

#ifdef KA_OS_WIN
#include <Winsock2.h>
#endif // KA_OS_WIN


KA_NAMESPACE_BEGIN

struct Application::Private
{
	Server server;
	int maxUserNum;
	ushort serverPort;

	Private()
		: maxUserNum(2)
		, serverPort(2610)
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
		}
	}

#ifdef KA_OS_WIN
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 0);
	WSAStartup(sockVersion, &wsaData);
#endif // KA_OS_WIN
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

	std::thread daemon([&]() {
		for (;;) {
			User *user = d->server.next();
			if (user == nullptr) {
				break;
			}
		}
	});

	std::string command;
	while (std::cin >> command) {
		std::cout << "error" << std::endl;
	}
	
	d->server.close();
	daemon.join();
	return 0;
}

KA_NAMESPACE_END
