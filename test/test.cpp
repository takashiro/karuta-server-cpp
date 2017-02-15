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

static std::vector<UnitTestObject> &UnitTests()
{
	static std::vector<UnitTestObject> test;
	return test;
}

void AddUnitTest(const char *name, void(*func)())
{
	std::vector<UnitTestObject> &tests = UnitTests();
	tests.push_back({name, func});
	std::cout << "Unit Test " << tests.size() << ": " << name << std::endl;
}

int main(int argc, const char *argv[])
{
	srand(static_cast<uint>(time(nullptr)));

	const std::vector<UnitTestObject> &tests = UnitTests();
	std::cout << "====== Start Unit Test ======" << std::endl;

	if (argc <= 1) {
		for (const UnitTestObject &unit : tests) {
			std::cout << "Testing " << unit.name << " ..." << std::endl;
			(*(unit.func))();
			std::cout << "--------------------" << std::endl;
		}
	} else {
		for (int i = 1; i < argc; i++) {
			const char *target = argv[i];
			for (const UnitTestObject &unit : tests) {
				if (unit.name != target) {
					continue;
				}
				std::cout << "Testing " << unit.name << "..." << std::endl;
				(*(unit.func))();
				goto NextUnit;
			}
			std::cout << target << " not found." << std::endl;
			NextUnit: std::cout << "--------------------" << std::endl;
		}
	}

	std::cout << "===== Unit Test Done! =====" << std::endl;
	return 0;
}

#endif
