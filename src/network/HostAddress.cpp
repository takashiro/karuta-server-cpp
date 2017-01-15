#include "HostAddress.h"

KA_NAMESPACE_BEGIN

HostAddress HostAddress::Any(0);
HostAddress HostAddress::Local(127, 0, 0, 1);

HostAddress::HostAddress(uchar ip1, uchar ip2, uchar ip3, uchar ip4)
{
	mIp4[0] = ip1;
	mIp4[1] = ip2;
	mIp4[2] = ip3;
	mIp4[3] = ip4;
}

KA_NAMESPACE_END
