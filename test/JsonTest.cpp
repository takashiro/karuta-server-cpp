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

	TEST_METHOD(IOIntegerTest)
	{
		int number = 1234567890;
		Json in(number);
		std::stringstream ss;
		ss << in;
		Json out;
		ss >> out;
		assert(out.type() == Json::Integer);
		assert(number == out.toInt());
	}

	TEST_METHOD(IODoubleTest)
	{
		double number = 1234.56;
		Json in(number);
		std::stringstream ss;
		ss << in;
		Json out;
		ss >> out;
		assert(out.type() == Json::Double);
		std::cout << out.toDouble() << std::endl;
		assert(number == out.toDouble());
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

	TEST_METHOD(BrokenArrayTest1)
	{
		std::stringstream ss;
		ss << "[1,2,3,4,5";
		Json json;
		ss >> json;
		assert(json.isArray());
		for(int i = 0; i < 5; i++) {
			assert(json[i].toInt() == i + 1);
		}
	}

	TEST_METHOD(BrokenArrayTest2)
	{
		std::stringstream ss;
		ss << "[1,2,3,4,5,asdfjb";
		Json json;
		ss >> json;
		assert(json.isArray());
		for(int i = 0; i < 5; i++) {
			assert(json[i].toInt() == i + 1);
		}
	}

	TEST_METHOD(BrokenObjectTest1)
	{
		std::stringstream ss;
		ss << "{1994, 1010";
		Json json;
		ss >> json;
		assert(json.isObject());
		assert(json.size() == 0);
	}

	TEST_METHOD(BrokenObjectTest2)
	{
		std::stringstream ss;
		ss << "{\"fang\" : 1994, \"crasteham\" : 1010, 123";
		Json json;
		ss >> json;
		assert(json.isObject());
		assert(json["fang"].toInt() == 1994);
		assert(json["crasteham"].toInt() == 1010);
	}

	TEST_METHOD(RandomStringTest)
	{
		for (int t = 0; t < 100; t++) {
			std::stringstream ss;
			for (int i = 0; i < 10; i++) {
				ss.put(rand());
			}
			Json json;
			ss >> json;
		}
	}

};

}
