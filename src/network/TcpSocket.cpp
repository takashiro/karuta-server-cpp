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

TcpSocket::TcpSocket()
{
}

TcpSocket::~TcpSocket()
{
}

bool TcpSocket::open(const HostAddress &ip, ushort port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = static_cast<uint32>(ip);

	SOCKET native = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == ::connect(native, reinterpret_cast<sockaddr *>(&addr), sizeof(addr))) {
		return false;
	}
	setNativeSocket(&native);
	return true;
}

KA_NAMESPACE_END
