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
#elif defined(KA_OS_LINUX)
#include <dlfcn.h>
#else
#error "Unsupported operating system."
#endif

KA_NAMESPACE_BEGIN

struct GameLoader::Private
{
	GameDriver *driver;
#if defined(KA_OS_WIN)
	HMODULE dll;
#elif defined(KA_OS_LINUX)
	void *dll;
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
	int raw_length = static_cast<int>(strlen(name));
	int length = MultiByteToWideChar(CP_UTF8, 0, name, raw_length, nullptr, 0) + 1;
	wchar_t *module_path = new wchar_t[length]();
	MultiByteToWideChar(CP_UTF8, 0, name, raw_length, module_path, length);
	d->dll = LoadLibrary(module_path);
	delete[] module_path;
# else
	d->dll = LoadLibrary(name);
# endif //UNICODE
#elif defined(KA_OS_LINUX)
	std::string libname = "./lib";
	libname += name;
	libname += ".so";
	d->dll = dlopen(libname.c_str(), RTLD_LAZY);
#endif

	using Loader = GameDriver *(*)();
#if defined(KA_OS_WIN)
	Loader loader = reinterpret_cast<Loader>(GetProcAddress(d->dll, "LoadGameDriver"));
#elif defined(KA_OS_LINUX)
	Loader loader = reinterpret_cast<Loader>(dlsym(d->dll, "LoadGameDriver"));
#endif
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
#if defined(KA_OS_WIN)
		FreeLibrary(d->dll);
#elif defined(KA_OS_LINUX)
		dlclose(d->dll);
#endif
	}
	delete d;
}

GameDriver *GameLoader::driver()
{
	return d->driver;
}

KA_NAMESPACE_END
