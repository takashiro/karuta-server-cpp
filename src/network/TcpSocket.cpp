#include "TcpSocket.h"

#include <WinSock2.h>

BA_NAMESPACE_BEGIN

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
		WSACleanup();
		return false;
	}
	setNativeSocket(&native);
	return true;
}

BA_NAMESPACE_END
