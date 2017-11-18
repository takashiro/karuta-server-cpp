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

#include <Event.h>
#include <EventLoop.h>

#include <thread>

namespace UnitTest
{
	class IntAddEvent : public Event
	{
	public:
		IntAddEvent(int num1, int num2, int &result)
			: num1(num1)
			, num2(num2)
			, result(result)
		{
		}

		void exec() override
		{
			result = num1 + num2;
		}

	private:
		int num1;
		int num2;
		int &result;
	};

	class ForLoopEvent : public Event
	{
	public:
		ForLoopEvent(int num, int limit, int &result)
			: num(num)
			, limit(limit)
			, result(result)
		{
		}

		void exec() override
		{
			for (int i = 0; i < limit; i++) {
				result += num;
			}
		}

	private:
		int num;
		int limit;
		int &result;
	};

	TEST_CLASS(EventLoopUnit)
	{
	public:

		TEST_METHOD(PushTest)
		{
			EventLoop loop;
			std::thread loop_thread([&] () {
				loop.exec();
			});

			int result = 0;
			loop.push(new IntAddEvent(1, 2, result));

			loop.terminate();
			loop_thread.join();

			assert(result == 3);
		}

		TEST_METHOD(ParallelTest)
		{
			EventLoop loop;
			std::thread loop_thread([&] () {
				loop.exec();
			});

			int num = 1, limit = 10000000;
			int result = 0;
			auto worker = [&] () {
				loop.push(new ForLoopEvent(num, limit, result));
			};

			std::thread *worker_thread[50];
			for (int i = 0; i < 50; i++) {
				worker_thread[i] = new std::thread(worker);
			}
			for (int i = 0; i < 50; i++) {
				worker_thread[i]->join();
				delete worker_thread[i];
			}

			loop.terminate();
			loop_thread.join();

			assert(result = 10 * limit * num);
		}

		TEST_METHOD(ParallelPushTest)
		{
			EventLoop loop;
			std::thread loop_thread([&] () {
				loop.exec();
			});

			int result = 0;
			std::thread worker([&] () {
				for (int i = 0; i < 1000; i++) {
					loop.push(new IntAddEvent(1, 1, result));
				}
				loop.terminate();
			});

			worker.join();
			loop_thread.join();
		}

	};

}
