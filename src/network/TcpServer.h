#pragma once

#include "HostAddress.h"

KA_NAMESPACE_BEGIN

class TcpSocket;

class TcpServer
{
public:
	TcpServer();
	~TcpServer();

	bool listen(const HostAddress &ip, ushort port);
	void close();

	TcpSocket *next();

private:
	KA_DECLARE_PRIVATE
};

KA_NAMESPACE_END
