#include "test.h"

#include <time.h>

namespace
{
	class RandInit
	{
	public:
		RandInit()
		{
			srand(static_cast<uint>(time(nullptr)));
		}
	};
	RandInit init;
}
