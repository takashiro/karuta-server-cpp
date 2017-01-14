#include "test.h"

#include <time.h>
#include <thread>
#include <mutex>

#include <network/TcpServer.h>
#include <network/TcpSocket.h>
#include <network/WebSocket.h>

namespace UnitTest
{
	TEST_CLASS(Network)
	{
	public:

		TEST_METHOD(TcpSocketTest)
		{
			ushort port = static_cast<ushort>(rand(1000, 65535));

			TcpServer server;
			assert(server.listen(HostAddress::Any, port));

			TcpSocket client;
			assert(client.open(HostAddress::Local, port));

			TcpSocket *host = server.next();
			assert(host != nullptr);

			std::string request = "Hello, I love Sanguosha.";
			assert(client.write(request) == request.size());

			std::string received_request = host->read(request.size());
			assert(received_request == request);

			std::string response = "Great! I love it, too!";
			host->write(response);

			std::string received_response = client.read(response.size());
			assert(received_response == response);

			client.close();
			server.close();
		}

		TEST_METHOD(WebSocketTest)
		{
			ushort port = static_cast<ushort>(rand(1000, 65535));

			TcpServer server;
			assert(server.listen(HostAddress::Any, port));
			WebSocket client;
			bool client_connected = false;

			std::string request = "I have an apple.";
			std::string response = "I have a pen.";

			std::string received_request;
			std::string received_response;

			std::thread client_thread([&]() {
				if (client.open(HostAddress::Local, port)) {
					client.write(request);
					received_response = client.read();
				}
			});

			std::thread server_thread([&]() {
				TcpSocket *tcp_host = server.next();
				if (tcp_host == nullptr) {
					return;
				}

				WebSocket host(tcp_host);
				if (!host.isConnected()) {
					return;
				}

				received_request = host.read();
				host.write(response);
			});

			client_thread.join();
			server_thread.join();

			assert(received_request == request);
			assert(received_response == response);

			server.close();
			client.close();
		}
	};
}
