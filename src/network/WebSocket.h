#pragma once

#include "HostAddress.h"

#include <string>

KA_NAMESPACE_BEGIN

class TcpSocket;

class WebSocket
{
public:
	WebSocket();
	WebSocket(TcpSocket *socket);
	~WebSocket();

	bool open(const HostAddress &ip, ushort port);
	void close();

	std::string read();
	bool write(const std::string &message, bool masked = true);

	bool isConnected() const;

private:
	KA_DECLARE_PRIVATE
};

KA_NAMESPACE_END
