#pragma once

#include "global.h"

KA_NAMESPACE_BEGIN

namespace net {

enum NetworkCommand
{
	Invalid,

	Login,
	Logout,

	Speak,

	NetworkCommandCount
};

}

KA_NAMESPACE_END
