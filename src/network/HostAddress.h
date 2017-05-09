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

KA_NAMESPACE_BEGIN

struct KA_DLL_EXPORT HostAddress
{
	HostAddress() : mIp4Value(0) {}
	HostAddress(uint32 ip);
	HostAddress(uchar ip1, uchar ip2, uchar ip3, uchar ip4);

	operator uint32() const { return mIp4Value; }
	HostAddress &operator = (HostAddress value) { mIp4Value = value.mIp4Value; return *this; }

	static const uint32 Any = 0x00000000;
	static const uint32 Local = 0x7F000001;

private:
	union
	{
		uchar mIp4[4];
		uint32 mIp4Value;
	};

};

KA_NAMESPACE_END
