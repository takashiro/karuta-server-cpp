#pragma once

#include "global.h"

BA_NAMESPACE_BEGIN

struct HostAddress
{
	HostAddress() : mIp4Value(0) {}
	HostAddress(uint32 ip) : mIp4Value(ip) {}
	HostAddress(uchar ip1, uchar ip2, uchar ip3, uchar ip4);

	operator uint32() const { return mIp4Value; }
	HostAddress &operator = (HostAddress value) { mIp4Value = value.mIp4Value; return *this; }

	static HostAddress Any;
	static HostAddress Local;

private:
	union
	{
		uchar mIp4[4];
		uint32 mIp4Value;
	};

};

BA_NAMESPACE_END
