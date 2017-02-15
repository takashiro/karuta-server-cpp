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
