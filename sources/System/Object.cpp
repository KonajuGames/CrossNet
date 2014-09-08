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

#include "CrossNetRuntime/System/Object.h"

#include "CrossNetRuntime/System/String.h"

void * * System::Object::s__InterfaceMap__ = NULL;

void System::Object::__RegisterId__()
{
    s__InterfaceMap__ = CrossNetRuntime::InterfaceMapper::RegisterObjectStaticId(__GetId__(), sizeof(System::Object));
}

// Will have to be implemented somewhere else... (Once System::Type is defined...)
System::String * System::Object::ToString()
{
//    return (GetType()->get_FullName());
    return (System::String::Empty);
}

void * System::Object::__Cast__(int iid)
{
    void * * interfaceMap = m__InterfaceMap__;

    int *   interfaceList;
    int     numComparisons;

    // Although we could avoid this, we segregate the lists by interfaces and objects
    // I'm wondering if that's really necessary though...
    // We can then quickly see the corresponding interfaces and types for a given type but at the same time
    // given that the IID are positive for interfaces and negative for everything else, it should be easy anyway

    // Anyway, it seems that with this simple test we can actually divide the space by 2, and have a reasonable number
    // of comparison, so it's not that wasted... It also gives more usage for a fast comparison (as opposed to a slower binary comparison).
    // In any case, this implementation is slightly more optimized for interface look up.

    if (iid > 0)
    {
        // We are looking for a cast with interfaces
        numComparisons = CrossNetRuntime::InterfaceMapper::GetNumInterfaces(interfaceMap);
        interfaceList = CrossNetRuntime::InterfaceMapper::GetInterfaceList(interfaceMap);
    }
    else
    {
        // We are looking for a cast with classes

        // Before we compare all the base implementations, let's compare the current implementation
        // As that's the most probable cast - and is not in the implementation list anyway...
        if (iid == CrossNetRuntime::InterfaceMapper::GetId(interfaceMap))
        {
            return (this);
        }

        // No luck, let's see if we can compare with the base implementations
        int numInterfaces = CrossNetRuntime::InterfaceMapper::GetNumInterfacesAndClasses(interfaceMap, &numComparisons);
        interfaceList = CrossNetRuntime::InterfaceMapper::GetObjectList(interfaceMap, numInterfaces);
    }

    // I tried several different thing to optimize that, and they all failed compared to the simple C++ code
    // Duff's device killed the performance (it was 50% slower than just the second loop)
    // Assembly with repne scas edi was dead slow
    // Unrolling the loop was inneficient, etc...
    // On PowerPC the behavior might be different though

    while (numComparisons-- > 0)
    {
        if (*interfaceList-- == iid)
        {
            // Found it!
            return (this);
        }
    }

    // Before we leave, we test with System::Object (as it is never added to the interface map as it's implicit).
    // This case should happen very _rarely_ as crossnet should detect thoses and not do them
    // But this can happen during asserts or possibly some generic operations (would have to prove it though).
    if (iid == System::Object::__GetId__())
    {
        return (this);
    }

    // Didn't find what we were looking for...
    return (NULL);
}
