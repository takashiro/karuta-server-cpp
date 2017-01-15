#pragma once

#include "AbstractSocket.h"

#include <sstream>
#include <WinSock2.h>

KA_NAMESPACE_BEGIN

struct AbstractSocket::Private
{
	SOCKET socket;
	std::stringstream buffer;

	int available()
	{
		int offset = static_cast<int>(buffer.tellg());
		buffer.seekg(0, std::ios::end);
		int end = static_cast<int>(buffer.tellg());
		buffer.seekg(offset, std::ios::beg);
		return end - offset;
	}
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

bool AbstractSocket::waitForRead(int mseconds) const
{
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(d->socket, &fds);
	timeval timeout;
	timeout.tv_sec = mseconds / 1000;
	timeout.tv_usec = (mseconds % 1000) * 1000;
	return select(FD_SETSIZE, &fds, nullptr, nullptr, &timeout) > 0;
}

uint64 AbstractSocket::read(char *buffer, uint64 size)
{
	if (d->available() <= 0) {
		return recv(d->socket, buffer, static_cast<int>(size), MSG_WAITALL);
	} else {
		uint64 read = d->buffer.readsome(buffer, size);
		if (read < size) {
			read += recv(d->socket, buffer + read, static_cast<int>(size - read), MSG_WAITALL);
		}
		return size;
	}
}

std::string AbstractSocket::read(uint64 size)
{
	char *buffer = new char[size];
	uint64 len = read(buffer, size);

	std::string message;
	if (len >= 0) {
		message.append(buffer, len);
	}

	delete[] buffer;
	return message;
}

uint64 AbstractSocket::readLine(char *buffer, uint64 size)
{
	int endl = 0;

	if (d->available() > 0) {
		d->buffer.getline(buffer, size);
		endl = static_cast<int>(d->buffer.gcount());

		uint64 str_length = strlen(buffer);
		if (endl > str_length) {
			uint64 last = str_length - 1;
			if (buffer[last] == '\r') {
				buffer[last] = '\0';
				str_length--;
			}
			return str_length;
		}
	}

	while (endl < size) {
		int received = recv(d->socket, buffer + endl, static_cast<int>(size - endl), 0);
		if (received <= 0) {
			int errcode = WSAGetLastError();
			return 0;
		}

		received += endl;
		for (; endl < received; endl++) {
			if (buffer[endl] == '\n') {
				int new_offset = endl + 1;
				buffer[endl] = '\0';
				if (endl > 0 && buffer[endl - 1] == '\r') {
					endl--;
					buffer[endl] = '\0';
				}
				if (new_offset < received) {
					const char *rest = buffer + new_offset;
					int length = received - new_offset;
					d->buffer.write(rest, length);
				}
				return endl;
			}
		}
	}

	return 0;
}

std::string AbstractSocket::readLine(uint64 size)
{
	char *buffer = new char[size];
	uint64 length = readLine(buffer, size);
	if (length > 0) {
		std::string line(buffer, length);
		delete[] buffer;
		return line;
	}
	delete[] buffer;
	return std::string();
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

KA_NAMESPACE_END
