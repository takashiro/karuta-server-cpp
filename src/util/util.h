#pragma once

#include "global.h"

#include <string>

KA_NAMESPACE_BEGIN

std::string base64_encode(const uchar *data, uint64 length);
std::string base64_encode(const std::string &str);

std::string sha1(const char *data, uint64 length);
std::string sha1(const std::string &str);

KA_NAMESPACE_END
