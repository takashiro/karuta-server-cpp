#pragma once

#include "HostAddress.h"
#include "AbstractSocket.h"

#include <string>

BA_NAMESPACE_BEGIN

class TcpSocket : public AbstractSocket
{
public:
	TcpSocket();
	~TcpSocket();

	bool open(const HostAddress &ip, ushort port);
};

BA_NAMESPACE_END
