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

#ifndef __SYSTEM_TEXT_STRINGBUILDER_H__
#define __SYSTEM_TEXT_STRINGBUILDER_H__

#include "CrossNetRuntime/System/Delegate.h"
#include <vector>

namespace System
{
    class IFormatProvider;
    struct Decimal;

    namespace Text
    {
        class StringBuilder : public System::Object
        {
        public:
            CN_DYNAMIC_ID()

            static StringBuilder * __Create__();
            static StringBuilder * __Create__(System::Int32 capacity);
            static StringBuilder * __Create__(System::String *);
            static StringBuilder * __Create__(System::Int32 capacity, System::Int32 maxGrow);
            static StringBuilder * __Create__(System::String *, System::Int32 capacity);
            static StringBuilder * __Create__(System::String *, System::Int32 start, System::Int32 length, System::Int32 capacity);

            virtual System::String *    ToString();
            System::String *    ToString(System::Int32, System::Int32);

            StringBuilder * Append(System::Boolean);
            StringBuilder * Append(System::Byte);
            StringBuilder * Append(System::Char);
            StringBuilder * Append(System::Array__G<System::Char> *);
            StringBuilder * Append(System::Decimal);
            StringBuilder * Append(System::Double);
            StringBuilder * Append(System::Int16);
            StringBuilder * Append(System::Int32);
            StringBuilder * Append(System::Int64);
            StringBuilder * Append(System::Object * text);
            StringBuilder * Append(System::SByte);
            StringBuilder * Append(System::Single);
            StringBuilder * Append(System::String * text);
            StringBuilder * Append(System::UInt16);
            StringBuilder * Append(System::UInt32);
            StringBuilder * Append(System::UInt64);
            StringBuilder * Append(System::Char c, System::Int32);
            StringBuilder * Append(System::Array__G<System::Char> *, System::Int32, System::Int32);
            StringBuilder * Append(System::String *, System::Int32, System::Int32);

            StringBuilder * AppendFormat(System::String *, System::Object *);
            StringBuilder * AppendFormat(System::String *, System::Array__G<System::Object *> *);
            StringBuilder * AppendFormat(System::IFormatProvider *, System::String *, System::Array__G<System::Object *> *);
            StringBuilder * AppendFormat(System::String *, System::Object *, System::Object *);
            StringBuilder * AppendFormat(System::String *, System::Object *, System::Object *, System::Object *);

            System::Int32   get_Length();
            System::Int32   set_Length(System::Int32);
            System::Int32   get_Capacity();
            void            set_Capacity(System::Int32);
            System::Int32   get_MaxCapacity();
            StringBuilder * Replace(System::Char, System::Char);
            StringBuilder * Replace(System::String *, System::String *);
            StringBuilder * Replace(System::Char, System::Char, System::Int32, System::Int32);
            StringBuilder * Replace(System::String *, System::String *, System::Int32, System::Int32);
            System::Char get_Item(System::Int32);
            System::Char set_Item(System::Int32, System::Char);

            StringBuilder * Insert(System::Int32, System::Boolean);
            StringBuilder * Insert(System::Int32, System::Byte);
            StringBuilder * Insert(System::Int32, System::Char);
            StringBuilder * Insert(System::Int32, System::Array__G<System::Char> *);
            StringBuilder * Insert(System::Int32, System::Decimal);
            StringBuilder * Insert(System::Int32, System::Double);
            StringBuilder * Insert(System::Int32, System::Int16);
            StringBuilder * Insert(System::Int32, System::Int32);
            StringBuilder * Insert(System::Int32, System::Int64);
            StringBuilder * Insert(System::Int32, System::Object *);
            StringBuilder * Insert(System::Int32, System::SByte);
            StringBuilder * Insert(System::Int32, System::Single);
            StringBuilder * Insert(System::Int32, System::String);
            StringBuilder * Insert(System::Int32, System::UInt16);
            StringBuilder * Insert(System::Int32, System::UInt32);
            StringBuilder * Insert(System::Int32, System::UInt64);
            StringBuilder * Insert(System::Int32, System::String *, Int32);
            StringBuilder * Insert(System::Int32, System::Array__G<System::Char> *, System::Int32, System::Int32);
            StringBuilder * Remove(System::Int32 index, System::Int32 length);

            System::Int32   EnsureCapacity(System::Int32 capacity);

        private:
            StringBuilder();
            void    Reserve(System::Int32 newSize);

            static const int MIN_CAPACITY = 16;

            System::Char *  mBuffer;
            System::Int32   mSize;
            System::Int32   mCapacity;
        };
    }
}

#endif