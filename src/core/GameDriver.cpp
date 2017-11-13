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

#include "GameDriver.h"

#include <thread>

KA_NAMESPACE_BEGIN

struct GameDriver::Private
{
	std::string name;
	Room *room;
	std::thread *thread;

	Private()
		: room(nullptr)
		, thread(nullptr)
	{
	}
};

GameDriver::GameDriver()
	: d(new Private)
{
}

GameDriver::~GameDriver()
{
	delete d;
}

void GameDriver::setName(const std::string &name)
{
	d->name = name;
}

const std::string &GameDriver::name() const
{
	return d->name;
}

void GameDriver::setRoom(Room *room)
{
	d->room = room;
}

Room *GameDriver::room()
{
	return d->room;
}

void GameDriver::wait()
{
	if (d->thread) {
		d->thread->join();
		delete d->thread;
		d->thread = nullptr;
	}
}

void GameDriver::start()
{
	d->thread = new std::thread([this] () {
		trigger(started);
		run();
		trigger(ended);
	});
}

KA_NAMESPACE_END
