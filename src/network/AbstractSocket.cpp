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

#include "AbstractSocket.h"

#include <sstream>
#include <string.h>

#ifdef KA_OS_WIN
#include <WinSock2.h>
typedef int socksize_t;
#elif defined(KA_OS_LINUX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
typedef int SOCKET;
typedef ssize_t socksize_t;
#endif

KA_NAMESPACE_BEGIN

struct AbstractSocket::Private
{
	SOCKET socket;
	std::stringstream buffer;

	int available()
	{
		int offset = static_cast<int>(buffer.tellg());
		if (offset == -1) {
			return -1;
		}
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
#ifdef KA_OS_WIN
	closesocket(d->socket);
#elif defined(KA_OS_LINUX)
	::close(d->socket);
#endif
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
		socksize_t result = recv(d->socket, buffer, static_cast<int>(size), MSG_WAITALL);
		return result <= 0 ? 0 : result;
	} else {
		uint64 read = d->buffer.readsome(buffer, size);
		if (read < size) {
			socksize_t result = recv(d->socket, buffer + read, static_cast<int>(size - read), MSG_WAITALL);
			if (result > 0) {
				read += result;
			}
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
	uint64 endl = 0;

	if (d->available() > 0) {
		d->buffer.getline(buffer, size);
		endl = d->buffer.gcount();

		uint64 str_length = strlen(buffer);
		if (endl > str_length) {
			uint64 last = str_length - 1;
			if (buffer[last] == '\r') {
				buffer[last] = '\0';
				str_length--;
			}
			return str_length;
		} else {
			d->buffer.str(std::string());
			d->buffer.clear();
		}
	}

	while (endl < size) {
		uint64 received = 0;
		socksize_t result = recv(d->socket, buffer + endl, static_cast<int>(size - endl), 0);
		if (result <= 0) {
			//int errcode = WSAGetLastError();
			return 0;
		}
		received = result;

		received += endl;
		for (; endl < received; endl++) {
			if (buffer[endl] == '\n') {
				uint64 new_offset = endl + 1;
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
		if (result == -1) {
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
