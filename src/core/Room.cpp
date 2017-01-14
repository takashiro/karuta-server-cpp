#include "Room.h"

BA_NAMESPACE_BEGIN

struct Room::Private
{
	uint id;
};

Room::Room(uint id)
	: d(new Private)
{
	d->id = id;
}

Room::~Room()
{
	delete d;
}

BA_NAMESPACE_END
