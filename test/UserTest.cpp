#include "test.h"

#include <time.h>
#include <thread>
#include <mutex>

#include <core/User.h>
#include <util/Json.h>

#include <network/TcpServer.h>
#include <network/WebSocket.h>

namespace UnitTest {

static int result = 0;
static void TestAction(User *user, const Json &arguments)
{
	if (user) {
		result = arguments.toInt();
	}
}

TEST_CLASS(ServerUser)
{
public:

	TEST_METHOD(UserActionTest)
	{
		ushort port = static_cast<ushort>(rand(1000, 65535));
		int number = rand();

		std::map<int, User::Action> actions;
		actions[10] = TestAction;

		std::thread server_thread([&](){
			TcpServer server;
			if (!server.listen(HostAddress::Any, port)) {
				return;
			}
			TcpSocket *socket = server.next();
			WebSocket web_socket(socket);
			User user(&web_socket);
			user.setAction(&actions);
			user.exec();
		});

		std::thread client_thread([&]() {
			WebSocket socket;
			socket.open(HostAddress::Local, port);

			User user(&socket);
			user.notify(10, number);
			user.notify(0);
		});

		server_thread.join();
		client_thread.join();
		assert(result == number);
	}

};

}
