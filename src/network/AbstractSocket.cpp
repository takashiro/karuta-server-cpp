#pragma once

#include "AbstractSocket.h"

#include <WinSock2.h>

BA_NAMESPACE_BEGIN

struct AbstractSocket::Private
{
	SOCKET socket;
};

AbstractSocket::AbstractSocket()
	: d(new Private)
{
}

AbstractSocket::~AbstractSocket()
{
	delete d;
}

void AbstractSocket::close()
{
	closesocket(d->socket);
}

uint64 AbstractSocket::read(char * buffer, uint64 size)
{
	return recv(d->socket, buffer, static_cast<int>(size), 0);
}

std::string AbstractSocket::read(int size)
{
	char *buffer = new char[size];
	int len = recv(d->socket, buffer, size, 0);

	std::string message;
	if (len >= 0) {
		message.append(buffer, len);
	}

	delete[] buffer;
	return message;
}

uint64 AbstractSocket::write(const char *data, uint64 size)
{
	uint64 sent = 0;
	while (sent < size) {
		int result = ::send(d->socket, data, static_cast<int>(size - sent), 0);
		if (result == SOCKET_ERROR) {
			break;
		}
		sent += result;
	}
	return sent;
}

uint64 AbstractSocket::write(const std::string & data)
{
	return write(data.c_str(), data.size());
}

void AbstractSocket::setNativeSocket(void *native)
{
	SOCKET *socket = static_cast<SOCKET *>(native);
	d->socket = *socket;
}

void *AbstractSocket::nativeSocket() const
{
	return static_cast<void *>(&d->socket);
}

BA_NAMESPACE_END
