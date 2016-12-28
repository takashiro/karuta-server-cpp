#pragma once

#include "HostAddress.h"

BA_NAMESPACE_BEGIN

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
	BA_DECLARE_PRIVATE
};

BA_NAMESPACE_END
