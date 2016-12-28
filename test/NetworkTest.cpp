#include "CppUnitTest.h"
#include <network/TcpServer.h>
#include <network/TcpSocket.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
BA_USING_NAMESPACE

namespace UnitTest
{
	TEST_CLASS(Network)
	{
	public:
		TcpServer server;
		TcpSocket client;
		TcpSocket *host;

		TEST_METHOD(Tcp)
		{
			Assert::IsTrue(server.listen(HostAddress::Any, 52601));
			Assert::IsTrue(client.open(HostAddress::Local, 52601));

			host = server.next();
			Assert::IsNotNull(host);

			std::string request = "Hello, I love Sanguosha.";
			Assert::IsTrue(client.write(request) == request.size());

			std::string received_request = host->read(30);
			Assert::IsTrue(received_request == request);

			std::string response = "Great! I love it, too!";
			host->write(response);

			std::string received_response = client.read(30);
			Assert::IsTrue(received_response == response);

			client.close();
			server.close();
		}
	};
}
