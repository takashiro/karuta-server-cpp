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

#include "TcpServer.h"
#include "TcpSocket.h"

#ifdef KA_OS_WIN
#include <WinSock2.h>
#elif defined(KA_OS_LINUX)
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SOCKET int
#define INVALID_SOCKET (~0)
#define SOCKET_ERROR -1
#endif

KA_NAMESPACE_BEGIN

struct TcpServer::Private
{
	SOCKET socket;
	int maxClientNum;
	HostAddress ip;
	ushort port;

	Private()
		: socket(INVALID_SOCKET)
		, maxClientNum(SOMAXCONN)
	{
	}
};

TcpServer::TcpServer()
	: d(new Private)
{
}

TcpServer::~TcpServer()
{
	delete d;
}

bool TcpServer::listen(const HostAddress &ip, ushort port)
{
	d->socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (d->socket == INVALID_SOCKET) {
		return false;
	}

	sockaddr_in addr_sev;
	addr_sev.sin_family = AF_INET;
	addr_sev.sin_port = htons(port);
	addr_sev.sin_addr.s_addr = static_cast<uint32>(ip);
	if (SOCKET_ERROR == ::bind(d->socket, reinterpret_cast<sockaddr *>(&addr_sev), sizeof(addr_sev))) {
		return false;
	}

	if (SOCKET_ERROR == ::listen(d->socket, d->maxClientNum)) {
		return false;
	}

	d->ip = ip;
	d->port = port;
	return true;
}

void TcpServer::close()
{
#ifdef KA_OS_WIN
	closesocket(d->socket);
#elif defined KA_OS_LINUX
	shutdown(d->socket, SHUT_RDWR);
#endif
}

TcpSocket *TcpServer::next()
{
	sockaddr_in client_addr;
#if defined(KA_OS_LINUX)
	socklen_t len = static_cast<socklen_t>(sizeof(client_addr));
#elif defined(KA_OS_WIN)
	int len = static_cast<int>(sizeof(client_addr));
#endif

	SOCKET native_socket = accept(d->socket, reinterpret_cast<sockaddr *>(&client_addr), &len);
	if (native_socket == INVALID_SOCKET) {
		return nullptr;
	}

	TcpSocket *socket = new TcpSocket;
	socket->setNativeSocket(&native_socket);
	return socket;
}

KA_NAMESPACE_END
