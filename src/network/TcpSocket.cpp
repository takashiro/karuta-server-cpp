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
