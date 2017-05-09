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

#include "HostAddress.h"

KA_NAMESPACE_BEGIN

HostAddress::HostAddress(uint32 ip)
{
	const uchar *sec = reinterpret_cast<const uchar *>(&ip);
	mIp4[0] = sec[3];
	mIp4[1] = sec[2];
	mIp4[2] = sec[1];
	mIp4[3] = sec[0];
}

HostAddress::HostAddress(uchar ip1, uchar ip2, uchar ip3, uchar ip4)
{
	mIp4[0] = ip1;
	mIp4[1] = ip2;
	mIp4[2] = ip3;
	mIp4[3] = ip4;
}

KA_NAMESPACE_END
