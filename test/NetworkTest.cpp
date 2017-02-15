/***********************************************************************
KARUTA server
Copyright (C) 2016-2017  Kazuichi Takashiro

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

takashiro@qq.com
************************************************************************/

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

		TEST_METHOD(TcpSocketDisconnectTest)
		{
			ushort port = static_cast<ushort>(rand(100, 0xFFFF));
			int received = -1;

			TcpServer server;
			assert(server.listen(HostAddress::Any, port));

			std::thread server_thread([&]() {
				TcpSocket *socket = server.next();
				if (socket) {
					char buffer[2];
					received = socket->read(buffer, 2);
				}
			});

			std::thread client_thread([&]() {
				TcpSocket client;
				assert(client.open(HostAddress::Local, port));
				std::this_thread::sleep_for(std::chrono::seconds(1));
				client.close();
			});

			server_thread.join();
			client_thread.join();
			assert(received == 0);
		}

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

		TEST_METHOD(WebSocketDisconnectTest)
		{
			ushort port = static_cast<ushort>(rand(100, 0xFFFF));
			std::string received = "test";

			TcpServer server;
			assert(server.listen(HostAddress::Any, port));

			std::thread server_thread([&]() {
				TcpSocket *socket = server.next();
				if (socket) {
					WebSocket websocket(socket);
					received = websocket.read();
				}
			});

			std::thread client_thread([&]() {
				TcpSocket client;
				assert(client.open(HostAddress::Local, port));
				std::this_thread::sleep_for(std::chrono::seconds(1));
				client.close();
			});

			server_thread.join();
			client_thread.join();
			assert(received.empty());
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
