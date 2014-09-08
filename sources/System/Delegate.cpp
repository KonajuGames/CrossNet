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

#include "CrossNetRuntime/System/Delegate.h"

void * * System::Delegate::s__InterfaceMap__ = NULL;

void System::Delegate::__RegisterId__()
{
    s__InterfaceMap__ = CrossNetRuntime::InterfaceMapper::RegisterObject(sizeof(System::Delegate));
}

// For Combine and Remove we want to create something like a vector of delegate, so we can add and remove delegate as need...
// For the moment, it's simply not supported...

System::Delegate * System::Delegate::Combine(System::Delegate * a, System::Delegate * b)
{
    if (a == NULL)
    {
        return (b);
    }
    else if (b == a)
    {
        return (a);
    }
    return (a->CombineImpl(b));
}

System::Delegate * System::Delegate::Remove(System::Delegate * source, System::Delegate * value)
{
    if (source == NULL)
    {
        return (NULL);
    }
    else if (value == NULL)
    {
        return (source);
    }
    return (source->RemoveImpl(value));
}

System::Delegate * System::Delegate::CombineImpl(System::Delegate * /*other*/)
{
    CROSSNET_FAIL("This should never be executed - Multicast version should be called instead...");
    return (NULL);
}

System::Delegate * System::Delegate::RemoveImpl(System::Delegate * other)
{
    if (Equals(other))
    {
        // Matching, remove it...
        return (this);
    }
    // Not matching, don't remove it...
    return (NULL);
}