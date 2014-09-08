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

#ifndef __SYSTEM_COLLECTIONS_GENERIC_ICOLLECTION__G1_H__
#define __SYSTEM_COLLECTIONS_GENERIC_ICOLLECTION__G1_H__

namespace System
{
    template <typename T>
    class Array__G;

    namespace Collections
    {
        namespace Generic
        {
            // Can't be user defined as this interface is implemented by arrays
            // Definition complete
            template <typename T>
            class ICollection__G1 : public CrossNetRuntime::IInterface
            {
            public:
                CN_MULTIPLE_DYNAMIC_INTERFACE_ID(
                    __W2__
                    (
                        CN_INTERFACE(System::Collections::IEnumerable),
                        CN_INTERFACE(System::Collections::Generic::IEnumerable__G1<T>)
                    )
                )

                public:
                virtual ::System::Int32 get_Count(void * __instance__) = 0;
                public:
                virtual ::System::Boolean get_IsReadOnly(void * __instance__) = 0;
                public:
                virtual void Add(void * __instance__, T item) = 0;
                public:
                virtual void Clear(void * __instance__) = 0;
                public:
                virtual ::System::Boolean Contains(void * __instance__, T item) = 0;
                public:
                virtual void CopyTo(void * __instance__, typename ::System::Array__G< T > * array, ::System::Int32 arrayIndex) = 0;
                public:
                virtual ::System::Boolean Remove(void * __instance__, T item) = 0;
            };
        }
    }
}

#endif
