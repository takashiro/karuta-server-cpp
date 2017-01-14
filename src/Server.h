#pragma once

#include "network/HostAddress.h"

BA_NAMESPACE_BEGIN

class Server
{
public:
	Server();
	~Server();

	bool listen(const HostAddress &ip, ushort port);
	void close();

private:
	BA_DECLARE_PRIVATE
};

BA_NAMESPACE_END
