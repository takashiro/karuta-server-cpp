#pragma once

#include "global.h"

#ifdef KA_OS_WIN

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
KA_USING_NAMESPACE

#define assert(exp) Assert::IsTrue(exp)
#define alert(message) Logger::WriteMessage(message)

#elif defined(KA_OS_LINUX)

#include <assert.h>
#include <string>
#include <vector>
#include <iostream>

#define alert(message) std::cerr << message << std::endl

#define TEST_CLASS(classname) class classname

#define TEST_METHOD(methodname) void methodname()

#endif

inline int rand(int min, int max)
{
	return min + rand() % (max - min);
}

extern std::vector<void(*)()> UnitTests;
