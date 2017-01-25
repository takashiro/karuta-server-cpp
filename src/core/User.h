#pragma once

#include "global.h"

#include <map>
#include <functional>

KA_NAMESPACE_BEGIN

class Json;
class WebSocket;
class Semaphore;
struct HostAddress;

class User
{
public:
	User();
	User(WebSocket *socket);
	~User();

	bool connect(const HostAddress &ip, ushort port);

	typedef std::function<void(User *, const Json &)> Action;
	void setAction(const std::map<int, Action> *handlers);
	void exec();

	void disconnect();

	void notify(int command);
	void notify(int command, const Json &arguments);

	Json request(int command, const Json &arguments, int timeout = 0);
	void reply(int command, const Json &arguments);

	void prepareRequest(int command, const Json &arguments, int timeout = 0);
	bool executeRequest(const std::function<void(const Json &)> &callback);
	bool executeRequest(std::function<void(const Json &)> &&callback);
	Json getReply() const;

private:
	KA_DECLARE_PRIVATE
};

KA_NAMESPACE_END
