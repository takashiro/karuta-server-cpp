#pragma once

#include "global.h"

#include <string>

BA_NAMESPACE_BEGIN

class AbstractSocket
{
public:
	AbstractSocket();
	virtual ~AbstractSocket();

	void close();

	uint64 read(char *buffer, uint64 size);
	std::string read(int size);

	uint64 write(const char *data, uint64 size);
	uint64 write(const std::string &data);

	void setNativeSocket(void *native);
	void *nativeSocket() const;

private:
	BA_DECLARE_PRIVATE
};

BA_NAMESPACE_END
