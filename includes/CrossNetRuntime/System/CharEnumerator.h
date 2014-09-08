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

#ifndef __SYSTEM_CHARENUMERATOR_H__
#define __SYSTEM_CHARENUMERATOR_H__

// The user cannot override this type
#include "CrossNetRuntime/System/Object.h"

namespace System
{
    class String;

    class CharEnumerator : public System::Object
    {
    public:
        CN_DYNAMIC_ID()

        CROSSNET_FINLINE
        static CharEnumerator * __Create__(System::String * s)
        {
            CharEnumerator * instance = new CharEnumerator(s);
            instance->m__InterfaceMap__ = __GetInterfaceMap__();
            return (instance);
        }

        // Normally this constructor can only be accessed by the String
        // We could put is as private and use a friend class String.
        // But for the moment avoid this, it makes the stuff simpler and only the C++ code would be able to access it directly
        CROSSNET_FINLINE
        System::Char    get_Current();
        CROSSNET_FINLINE
        System::Boolean MoveNext();

        void            Reset()
        {
            mIndex = -1;
        }

    private:
        CROSSNET_FINLINE
        CharEnumerator(System::String * s);

        System::String *        mString;    // In reality we don't really need this as all the information are cached below
                                            // But we keep it so the corresponding string is not collected during the iteration...
        System::Int32           mIndex;
        System::Int32           mLength;
        const System::Char *    mBuffer;
    };
}

// Implementation of the inline functions

// Headers needed for the inline functions
#include "CrossNetRuntime/System/String.h"

namespace System
{
    CharEnumerator::CharEnumerator(System::String * s)
        :
        mString(s),
        mIndex(-1),
        mLength(s->get_Length()),
        mBuffer(s->__ToCString__())
    {
        // Do nothing...
    }

    System::Char    CharEnumerator::get_Current()
    {
        CROSSNET_ASSERT((mIndex >= 0) && (mIndex < mLength), "The index is outside the range!");
        return (mBuffer[mIndex]);
    }

    System::Boolean CharEnumerator::MoveNext()
    {
        ++mIndex;
        return (mIndex < mLength);
    }
}

#endif