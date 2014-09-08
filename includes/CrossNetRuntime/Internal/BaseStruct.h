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

#ifndef __CROSSNET_BASESTRUCT_H__
#define __CROSSNET_BASESTRUCT_H__

#include "CrossNetRuntime/Assert.h"

namespace CrossNetRuntime
{
    // Fake structure to handle "base.???" for structures...
    struct BaseStruct
    {
        static System::Boolean Equals(void * * /*interfaceMap*/, void * /*instance*/, System::Object * /*obj*/)
        {
            CROSSNET_FAIL("Implement this!");
            return (false);
        }

        static System::Boolean Equals(System::Object * /*obj*/)
        {
            CROSSNET_FAIL("Implement this!");
            return (false);
        }

        static System::Int32 GetHashCode(void * /*instance*/)
        {
            CROSSNET_FAIL("Implement this!");
            return (0);
        }

        static System::Int32 GetHashCode()
        {
            CROSSNET_FAIL("Implement this!");
            return (0);
        }

        static System::Type * GetType()
        {
            CROSSNET_FAIL("Implement this!");
            return (NULL);
        }
    };

    template <typename U>
    CROSSNET_FINLINE
    System::Boolean StructEquals(U * thisInstance, ::System::Object * obj)
    {
        BoxedObject<U> * other = AsCast<BoxedObject<U> >(obj);
        if (other == NULL)
        {
            return (false);
        }
        return (__memcmp__(thisInstance, other->GetUnboxedAddress(), sizeof(U)) == 0);
    }

    template <typename U>
    CROSSNET_FINLINE
    System::Int32 StructGetHashCode(U * thisInstance)
    {
        return (GetHashCode(thisInstance, sizeof(U)));
    }
}

#endif
