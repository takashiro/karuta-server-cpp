#pragma once

#include "global.h"
#include <map>

KA_NAMESPACE_BEGIN

class Json;
class WebSocket;

class User
{
public:
	User(WebSocket *socket);
	~User();

	typedef void(*Action)(User *, const Json &);
	void setAction(const std::map<int, Action> *handlers);
	void exec();

	void notify(int command);
	void notify(int command, const Json &arguments);

	Json request(int command, const Json &arguments, int timeout = 0);
	void reply(int command, const Json &arguments);

private:
	KA_DECLARE_PRIVATE
};

KA_NAMESPACE_END
