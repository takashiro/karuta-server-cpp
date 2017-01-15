#pragma once

#include "HostAddress.h"
#include "AbstractSocket.h"

#include <string>

KA_NAMESPACE_BEGIN

class TcpSocket : public AbstractSocket
{
public:
	TcpSocket();
	~TcpSocket();

	bool open(const HostAddress &ip, ushort port);
};

KA_NAMESPACE_END
