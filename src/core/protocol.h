#pragma once

#include "global.h"

BA_NAMESPACE_BEGIN

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

BA_NAMESPACE_END
