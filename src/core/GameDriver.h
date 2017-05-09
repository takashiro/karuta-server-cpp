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

#pragma once

#include "global.h"

#include <string>

KA_NAMESPACE_BEGIN

class KA_DLL_EXPORT GameDriver
{
public:
	virtual ~GameDriver() {}

	virtual void start() = 0;
	virtual void end() = 0;

	virtual void addPlayer(uint id, const std::string &screenName) = 0;
	virtual void removePlayer(uint id) = 0;
	virtual void modifyPlayer(uint id, const char *key, void *value) = 0;
};

KA_NAMESPACE_END
