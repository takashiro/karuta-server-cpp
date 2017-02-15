#pragma once

#ifdef KA_NAMESPACE
#define KA_NAMESPACE_BEGIN namespace KA_NAMESPACE {
#define KA_NAMESPACE_END }
#define KA_USING_NAMESPACE using namespace KA_NAMESPACE;
#else
#define KA_NAMESPACE_BEGIN
#define KA_NAMESPACE_END
#define KA_USING_NAMESPACE
#endif // KA_NAMESPACE

#define KA_DECLARE_PRIVATE struct Private; friend struct Private; Private *d;

//Operating System Macros
#if defined(_WIN32) || defined(_WIN64)
#define KA_OS_WIN
#elif defined(__linux__)
#define KA_OS_LINUX
#else
#error "Unsupported OS"
#endif

KA_NAMESPACE_BEGIN

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

KA_NAMESPACE_END
