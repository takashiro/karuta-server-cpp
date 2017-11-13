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

#include <functional>

KA_NAMESPACE_BEGIN

class KA_DLL_EXPORT Object
{
public:
	Object();
	~Object();

	using Signal = void(*)();
	using Slot = std::function<void()>;

	void bind(Signal signal, const Slot &slot);
	void bind(Signal signal, Slot &&slot);
	void unbind(Signal signal);

	static void test() {}

protected:
	void trigger(Signal signal);

private:
	KA_DECLARE_PRIVATE	
};

KA_NAMESPACE_END
