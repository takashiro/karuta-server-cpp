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

#include "Semaphore.h"

#include <mutex>
#include <condition_variable>

KA_NAMESPACE_BEGIN

struct Semaphore::Private
{
	std::mutex mutex;
	std::condition_variable cond;
	int count;
};

Semaphore::Semaphore(int num)
	: d(new Private)
{
	d->count = num;
}


Semaphore::~Semaphore()
{
	delete d;
}

void Semaphore::acquire(int num)
{
	std::unique_lock<std::mutex> lock(d->mutex);
	while (d->count < num) {
		d->cond.wait(lock);
	}
	d->count -= num;
}

bool Semaphore::acquire(int num, int seconds)
{
	std::unique_lock<std::mutex> lock(d->mutex);
	if (d->count < num) {
		std::cv_status status = d->cond.wait_for(lock, std::chrono::seconds(seconds));
		if (status == std::cv_status::no_timeout) {
			if (d->count >= num) {
				d->count -= num;
				return true;
			}
		}
		return false;
	} else {
		d->count -= num;
		return true;
	}
}

void Semaphore::release(int num)
{
	std::unique_lock<std::mutex> lock(d->mutex);
	d->count += num;
	d->cond.notify_one();
}

KA_NAMESPACE_END
