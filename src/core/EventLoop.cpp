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

#include "EventLoop.h"

#include "Event.h"
#include "Semaphore.h"

#include <mutex>
#include <queue>

KA_NAMESPACE_BEGIN

struct EventLoop::Private
{
	std::queue<Event *> events;
	Semaphore ready;
	std::mutex mutex;
};

EventLoop::EventLoop()
	: d(new Private)
{
}

EventLoop::~EventLoop()
{
	delete d;
}

void EventLoop::exec()
{
	for (;;) {
		d->ready.acquire(1);

		d->mutex.lock();
		Event *event = d->events.front();
		d->events.pop();
		d->mutex.unlock();

		if (event) {
			event->exec();
			delete event;
		} else {
			break;
		}
	}
}

void EventLoop::terminate()
{
	push(nullptr);
}

void EventLoop::push(Event *event)
{
	d->ready.release(1);
	d->mutex.lock();
	d->events.push(event);
	d->mutex.unlock();
}

KA_NAMESPACE_END
