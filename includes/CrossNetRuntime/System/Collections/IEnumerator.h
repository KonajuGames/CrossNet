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

#ifndef __SYSTEM_COLLECTIONS_IENUMERATOR_H__
#define __SYSTEM_COLLECTIONS_IENUMERATOR_H__

// The user cannot override this type
#include "CrossNetRuntime/Internal/IInterface.h"

namespace System
{
    class Object;

    namespace Collections
    {
        class IEnumerator : public CrossNetRuntime::IInterface
        {
        public:
            CN_DYNAMIC_INTERFACE_ID0()

            virtual bool                MoveNext(void * __instance__) = 0;
            virtual System::Object *    get_Current(void * __instance__) = 0;
            virtual void                Reset(void * __instance__) = 0;
        };
    }
}

#endif
