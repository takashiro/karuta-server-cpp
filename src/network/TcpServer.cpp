#include "TcpServer.h"
#include "TcpSocket.h"

#include <WinSock2.h>

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
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 0);
	if (WSAStartup(sockVersion, &wsaData)) {
		return false;
	}

	d->socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (d->socket == INVALID_SOCKET) {
		WSACleanup();
		return false;
	}

	sockaddr_in addr_sev;
	addr_sev.sin_family = AF_INET;
	addr_sev.sin_port = htons(port);
	addr_sev.sin_addr.s_addr = static_cast<uint32>(ip);
	if (SOCKET_ERROR == ::bind(d->socket, reinterpret_cast<sockaddr *>(&addr_sev), sizeof(addr_sev))) {
		WSACleanup();
		return false;
	}

	if (SOCKET_ERROR == ::listen(d->socket, d->maxClientNum)) {
		WSACleanup();
		return false;
	}

	d->ip = ip;
	d->port = port;
	return true;
}

void TcpServer::close()
{
	closesocket(d->socket);
}

TcpSocket *TcpServer::next()
{
	sockaddr_in client_addr;
	int len = sizeof(client_addr);

	SOCKET native_socket = accept(d->socket, reinterpret_cast<sockaddr *>(&client_addr), &len);
	if (native_socket == INVALID_SOCKET) {
		return nullptr;
	}

	TcpSocket *socket = new TcpSocket;
	socket->setNativeSocket(&native_socket);
	return socket;
}

KA_NAMESPACE_END
