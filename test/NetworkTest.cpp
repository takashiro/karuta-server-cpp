#include "CppUnitTest.h"

#include <time.h>
#include <thread>
#include <mutex>

#include <network/TcpServer.h>
#include <network/TcpSocket.h>
#include <network/WebSocket.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
BA_USING_NAMESPACE

namespace UnitTest
{
	TEST_CLASS(Network)
	{
	public:

		TEST_METHOD(TcpSocketTest)
		{
			TcpServer server;
			Assert::IsTrue(server.listen(HostAddress::Any, 52601));

			TcpSocket client;
			Assert::IsTrue(client.open(HostAddress::Local, 52601));

			TcpSocket *host = server.next();
			Assert::IsNotNull(host);

			std::string request = "Hello, I love Sanguosha.";
			Assert::IsTrue(client.write(request) == request.size());

			std::string received_request = host->read(request.size());
			Assert::IsTrue(received_request == request);

			std::string response = "Great! I love it, too!";
			host->write(response);

			std::string received_response = client.read(response.size());
			Assert::IsTrue(received_response == response);

			client.close();
			server.close();
		}

		TEST_METHOD(WebSocketTest)
		{
			srand(static_cast<uint>(time(nullptr)));

			TcpServer server;
			Assert::IsTrue(server.listen(HostAddress::Any, 52601));

			std::string request = "I have an apple.";
			std::string response = "I have a pen.";
			WebSocket client;
			bool client_connected = false;
			std::thread client_thread([&]() {
				Assert::IsTrue(client.open(HostAddress::Local, 52601));
				client.write(request);

				std::string received_response = client.read();
				Assert::IsTrue(received_response == response);
			});
			client_thread.detach();

			TcpSocket *tcp_host = server.next();
			Assert::IsNotNull(tcp_host);

			WebSocket host(tcp_host);
			Assert::IsTrue(host.isConnected());

			std::string received_request = host.read();
			Assert::IsTrue(received_request == request);

			host.write(response);

			server.close();
			client.close();
		}
	};
}
