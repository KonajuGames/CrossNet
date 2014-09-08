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

#ifndef __CROSSNETRUNTIME_H__
#define __CROSSNETRUNTIME_H__

#include "CrossNetRuntime/Internal/__Math__.h"
#include "CrossNetRuntime/Internal/BaseStruct.h"
#include "CrossNetRuntime/Internal/BaseTypes.h"
#include "CrossNetRuntime/Internal/Box.h"
#include "CrossNetRuntime/Internal/Cast.h"
#include "CrossNetRuntime/Internal/EnumWrapper.h"
#include "CrossNetRuntime/Internal/IInterface.h"
#include "CrossNetRuntime/Internal/PointerWrapper.h"
#include "CrossNetRuntime/Internal/Primitives.h"
#include "CrossNetRuntime/Internal/Typeof.h"
#include "CrossnetRuntime/Internal/Tracer.h"

#include "CrossNetRuntime/System/Object.h"
#include "CrossNetRuntime/System/Array.h"
#include "CrossNetRuntime/System/CharEnumerator.h"
#include "CrossNetRuntime/System/StringComparison.h"
#include "CrossNetRuntime/System/ValueType.h"
#include "CrossNetRuntime/System/Text/StringBuilder.h"
#include "CrossNetRuntime/System/MulticastDelegate.h"

#include "CrossNetRuntime/GC/GCManager.h"
#include "CrossNetRuntime/InterfaceMapper.h"
#include "CrossNetRuntime/InitOptions.h"

// CrossNet uses float macros for some of the float constants.
#include <float.h>
// Same for Int32 and Int64 min as some compiler have some issues with the constant as is
//  For example VC++ will issue a warning...
#include <limits.h>

void CrossNetRuntime__PopulateInterfaceMaps();

namespace CrossNetRuntime
{
    struct InitOptions;

    void Setup(const InitOptions & options);
    void Teardown();

    void Trace(unsigned char currentMark);

    System::Int32 GetHashCode(void * bufferToCrc, System::Int32 length);
    System::Int32 GetHashCodeForString(::System::Char * textToCrc, System::Int32 length);

    CROSSNET_FINLINE
    void SetFixed(void * /*ptr*/, System::Boolean /*fix*/)
    {
        // Do nothing...
        // TODO: Implement
    }
}

#include "CrossNetRuntime/StringPooler.h"

#endif

