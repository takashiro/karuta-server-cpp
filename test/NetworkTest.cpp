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
			srand(static_cast<uint>(time(nullptr)));
			ushort port = static_cast<ushort>(rand());

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
			srand(static_cast<uint>(time(nullptr)));
			ushort port = static_cast<ushort>(rand());

			TcpServer server;
			assert(server.listen(HostAddress::Any, port));

			std::string request = "I have an apple.";
			std::string response = "I have a pen.";
			WebSocket client;
			bool client_connected = false;
			std::thread client_thread([&]() {
				assert(client.open(HostAddress::Local, port));
				client.write(request);

				std::string received_response = client.read();
				assert(received_response == response);
			});
			client_thread.detach();

			TcpSocket *tcp_host = server.next();
			assert(tcp_host != nullptr);

			WebSocket host(tcp_host);
			assert(host.isConnected());

			std::string received_request = host.read();
			assert(received_request == request);

			host.write(response);

			server.close();
			client.close();
		}
	};
}
