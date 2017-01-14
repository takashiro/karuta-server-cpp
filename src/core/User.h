#pragma once

#include "global.h"
#include <map>

BA_NAMESPACE_BEGIN

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

private:
	BA_DECLARE_PRIVATE
};

BA_NAMESPACE_END
