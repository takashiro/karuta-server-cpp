#pragma once

#include "HostAddress.h"

#include <string>

BA_NAMESPACE_BEGIN

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
	BA_DECLARE_PRIVATE
};

BA_NAMESPACE_END
