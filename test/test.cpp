#include "test.h"

#include <time.h>
#include <WinSock2.h>

namespace
{
	class RandInit
	{
	public:
		RandInit()
		{
			srand(static_cast<uint>(time(nullptr)));
		}
	};
	RandInit init;
}

namespace UnitTest
{

	TEST_MODULE_INITIALIZE(Init)
	{
		WSADATA wsaData;
		WORD sockVersion = MAKEWORD(2, 0);
		WSAStartup(sockVersion, &wsaData);
	}

	TEST_MODULE_CLEANUP(Cleanup)
	{
		WSACleanup();
	}

}
