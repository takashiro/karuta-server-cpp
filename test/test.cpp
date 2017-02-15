#include "test.h"

#include <time.h>

#ifdef KA_OS_WIN

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

#elif defined(KA_OS_LINUX)

std::vector<void(*)()> UnitTests;

int main()
{
	srand(static_cast<uint>(time(nullptr)));

	for (void(*func)() : UnitTests) {
		(*func)();
	}

	puts("Unit tests done!");
	return 0;
}

#endif
