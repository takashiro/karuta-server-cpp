#pragma once

#ifdef BA_NAMESPACE
#define BA_NAMESPACE_BEGIN namespace BA_NAMESPACE {
#define BA_NAMESPACE_END }
#define BA_USING_NAMESPACE using namespace BA_NAMESPACE;
#else
#define BA_NAMESPACE_BEGIN
#define BA_NAMESPACE_END
#define BA_USING_NAMESPACE
#endif // BA_NAMESPACE

#define BA_DECLARE_PRIVATE struct Private; friend struct Private; Private *d;

BA_NAMESPACE_BEGIN

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ullong;

typedef short int16;
typedef ushort uint16;
typedef int int32;
typedef uint uint32;
typedef long long int64;
typedef ullong uint64;

BA_NAMESPACE_END
