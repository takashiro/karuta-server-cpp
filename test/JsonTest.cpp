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

#include <Json.h>

#include <sstream>

namespace UnitTest {

TEST_CLASS(JsonUnit)
{
public:

	TEST_METHOD(IntegerTest)
	{
		int number1 = rand();
		int number2 = rand();

        Json json1(number1);
        int result1 = json1.toInt();

        Json json2(number2);
        int result2 = json2.toInt();

		assert(result1 == number1);
		assert(result2 == number2);
	}

	TEST_METHOD(CopyConstructorTest)
	{
		Json json(617931000);
		Json json2(std::move(json));
		int result = json2.toInt();
		assert(result == 617931000);
	}

	TEST_METHOD(IOPreciseTest)
	{
		int number = 1234567890;
		Json in(number);
		std::stringstream ss;
		ss << in;
		Json out;
		ss >> out;
		assert(number == out.toInt());
	}

	TEST_METHOD(StringTest)
	{
		std::stringstream ss;
		ss << "\"\\\"So much bluuuuuue!!!\\\", Yunzhe Fang surprised.\"";
		Json json;
		ss >> json;
		assert(json.isString());
		std::string str = json.toString();
		assert(str == "\"So much bluuuuuue!!!\", Yunzhe Fang surprised.");
	}

};

}
