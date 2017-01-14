#include "test.h"

#include <time.h>
#include <thread>
#include <mutex>

#include <core/User.h>
#include <util/Json.h>

#include <network/TcpServer.h>
#include <network/WebSocket.h>

namespace UnitTest {

static int result1 = 0;
static void TestAction1(User *user, const Json &arguments)
{
	if (user) {
		result1 = arguments.toInt();
	}
}

static int result2 = 0;
static void TestAction2(User *user, const Json &arguments)
{
	if (user) {
		result2 = arguments.toInt();
	}
}

TEST_CLASS(ServerUser)
{
public:

	TEST_METHOD(UserActionTest)
	{
		ushort port = static_cast<ushort>(rand(1000, 65535));
		int number1 = rand();
		int number2 = rand();

		std::map<int, User::Action> actions;
		actions[10] = TestAction1;
		actions[1010] = TestAction2;

		TcpServer server;
		assert(server.listen(HostAddress::Any, port));

		std::thread server_thread([&](){
			TcpSocket *socket = server.next();
			WebSocket *websocket = new WebSocket(socket);
			User user(websocket);
			user.setAction(&actions);
			user.exec();
		});

		std::thread client_thread([&]() {
			WebSocket *socket = new WebSocket;
			if (!socket->open(HostAddress::Local, port)) {
				delete socket;
				alert("WebSocket failed to open");
				return;
			}
			User user(socket);
			user.notify(10, number1);
			user.notify(1010, number2);
			user.notify(0);
		});

		server_thread.join();
		client_thread.join();
		assert(result1 == number1);
		assert(result2 == number2);
	}

};

}
