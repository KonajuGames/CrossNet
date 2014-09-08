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

#include "CrossNetRuntime/StringPooler.h"
#include "CrossNetRuntime/GC/GCManager.h"
#include "CrossNetRuntime/System/String.h"

namespace CrossNetRuntime
{

StringPooler::StringHashMap   StringPooler::sAllStrings;

::System::String * StringPooler::GetOrCreateString(System::Char * text)
{
    System::Int32 length = (System::Int32)wcslen(text);
    Key k(text, length);
    StringHashMap::const_iterator it = sAllStrings.find(k);
    if (it != sAllStrings.end())
    {
        return (it->second);
    }

    // We did not find the string, so we can create it (and add it to the pool)

    ::System::String * str = ::System::String::__CreateWithLengthKnown__(text, length);
    sAllStrings[k] = str;
    return (str);
}

::System::String * StringPooler::GetOrCreateString(System::Char * text, System::Int32 length)
{
    Key k(text, length);
    StringHashMap::const_iterator it = sAllStrings.find(k);
    if (it != sAllStrings.end())
    {
        return (it->second);
    }

    // We did not find the string, so we can create it (and add it to the pool)

    ::System::String * str = ::System::String::__CreateWithLengthKnown__(text, length);
    sAllStrings[k] = str;
    return (str);
}

void    StringPooler::AddString(System::String * str)
{
    Key k(str->__ToCString__(), str->get_Length());
    sAllStrings[k] = str;
}

void StringPooler::Trace(unsigned char currentMark)
{
    StringHashMap::iterator it, itEnd;

    it = sAllStrings.begin();
    itEnd = sAllStrings.end();

    // Trace every string in the pool
    while (it != itEnd)
    {
        CrossNetRuntime::GCManager::Trace((*it).second, currentMark);
        ++it;
    }
}

}

