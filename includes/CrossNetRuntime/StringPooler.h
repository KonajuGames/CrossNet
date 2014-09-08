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

#ifndef __STRINGPOOLER_H__
#define __STRINGPOOLER_H__

#include <hash_map>
#include "CrossNetRuntime/Internal/Primitives.h"
#include "CrossnetRuntime/CrossNetRuntime.h"

namespace System
{
    class String;
}

namespace CrossNetRuntime
{
    class StringPooler
    {
    public:
        static ::System::String *   GetOrCreateString(System::Char * text);
        // This function should be used with strings that can potentially contain a zero character
        static ::System::String *   GetOrCreateString(System::Char * text, System::Int32 length);
        static void                 Trace(unsigned char currentMark);

    private:
        StringPooler();
        StringPooler(const StringPooler & other);
        ~StringPooler();
        StringPooler & operator=(StringPooler & other);

        static void                 AddString(System::String * str);

        struct Key
        {
            Key(::System::Char * buffer, ::System::Int32 length)
                :
                mBuffer(buffer),
                mLength(length)
            {
                // Do nothing...
            }

            ::System::Char *    mBuffer;
            ::System::Int32     mLength;
        };

        struct StringCompare
        {
            static const int bucket_size = 4;
            static const int min_buckets = 8;

            StringCompare()
            {
            }

            size_t operator()(const Key & key) const
            {
                return (::CrossNetRuntime::GetHashCodeForString(key.mBuffer, key.mLength));
            }

            bool operator()(const Key & key1, const Key & key2) const
            {
                // True to continue the search, false if we found the same element
                // So in clear, true if not equal, false if it is equal
                if (key1.mLength != key2.mLength)
                {
                    return (true);
                }
                // We just compared the length before, no need to compare the trailing '\0'
                bool equal = (__memcmp__(key1.mBuffer, key2.mBuffer, sizeof(::System::Char) * key1.mLength) == 0);
                return (equal == false);
            }
        };

        // Char * comes from static definition, so they are always in memory
        //  Obviously by duplicating the buffer with the strings, we are multiplying
        //  The memory by two for the strings.
        //  There are ways around that though.
        typedef stdext::hash_map<Key, ::System::String *, StringCompare>  StringHashMap;

        static StringHashMap    sAllStrings;

        friend class ::System::String;
    };
}

#endif

