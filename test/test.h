#pragma once

#include "global.h"

KA_USING_NAMESPACE

#ifdef KA_OS_WIN

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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

struct UnitTestObject
{
	std::string name;
	void(*func)();
};

void AddUnitTest(const char *name, void(*func)());
