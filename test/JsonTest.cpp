#include "test.h"

#include <util/Json.h>

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

};

}
