#pragma once

#include "CppUnitTest.h"
#include "global.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
BA_USING_NAMESPACE

#define assert(exp) Assert::IsTrue(exp)
#define alert(message) Logger::WriteMessage(message)

inline int rand(int min, int max)
{
	return min + rand() % (max - min);
}
