/*
    CrossNet - Copyright (c) 2007 Olivier Nallet

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
    OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __CROSSNET_PRIMITIVES_H__
#define __CROSSNET_PRIMITIVES_H__

#include "CrossNetRuntime/InterfaceMapper.h"

namespace System
{
    // Define the primitive types
	typedef bool Boolean;
	typedef __wchar_t Char;
    typedef int Int32;
    typedef signed long long Int64;
    typedef void Void;
	typedef unsigned char Byte;
	typedef signed char SByte;
	typedef unsigned short UInt16;
	typedef signed short Int16;
	typedef unsigned int UInt32;
	typedef unsigned long long UInt64;
	typedef float Single;
	typedef double Double;
    // Note that Decimal is handled as standard structure

    // Create fake type to handle typeof(void)
    struct __Void__
    {
        CN_DYNAMIC_OBJECT_ID0(0)    // Size doesn't matter as this object won't be manipulated...
    };

    // Other standard types
    class Object;
    class String;
    class Type;
    struct Decimal;
}

#endif
