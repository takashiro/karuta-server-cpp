#pragma once

#include "global.h"

KA_NAMESPACE_BEGIN

class Semaphore
{
public:
	Semaphore(int num = 0);
	~Semaphore();

	void acquire(int num = 1);
	bool acquire(int num, int seconds);

	void release(int num = 1);

private:
	KA_DECLARE_PRIVATE
};

KA_NAMESPACE_END
