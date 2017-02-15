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
