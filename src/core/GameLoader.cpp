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

#include "GameLoader.h"
#include "GameDriver.h"

#if defined(KA_OS_WIN)
#include <Windows.h>
#endif

KA_NAMESPACE_BEGIN

struct GameLoader::Private
{
	GameDriver *driver;
#if defined(KA_OS_WIN)
	HMODULE dll;
#endif

	Private()
		: driver(nullptr)
		, dll(0)
	{
	}
};

GameLoader::GameLoader(const char *name)
	: d(new Private)
{
#if defined(KA_OS_WIN)
# ifdef UNICODE
	int raw_length = strlen(name);
	int length = MultiByteToWideChar(CP_UTF8, 0, name, raw_length, nullptr, 0) + 1;
	wchar_t *module_path = new wchar_t[length]();
	MultiByteToWideChar(CP_UTF8, 0, name, raw_length, module_path, length);
	d->dll = LoadLibrary(module_path);
	delete[] module_path;
# else
	HMODULE d->extension = LoadLibrary(name);
# endif //UNICODE
#endif

	using Loader = GameDriver *(*)();
	Loader loader = reinterpret_cast<Loader>(GetProcAddress(d->dll, "LoadGameDriver"));
	if (loader) {
		d->driver = loader();
	}
}

GameLoader::~GameLoader()
{
	if (d->driver) {
		delete d->driver;
	}
	if (d->dll) {
		FreeLibrary(d->dll);
	}
	delete d;
}

GameDriver *GameLoader::driver()
{
	return d->driver;
}

KA_NAMESPACE_END
