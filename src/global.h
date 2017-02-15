/***********************************************************************
KARUTA server
Copyright (C) 2016-2017  Kazuichi Takashiro

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

takashiro@qq.com
************************************************************************/

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
