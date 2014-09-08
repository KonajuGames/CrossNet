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

#ifndef __ENUMWRAPPER_H__
#define __ENUMWRAPPER_H__

#include "CrossNetRuntime/Internal/Primitives.h"

#define BEGIN_DECLARE_ENUM(name, type, def) struct name : public CrossNetRuntime::StructEnum<type, name>   \
                                            {                                                               \
                                            public:                                                         \
                                                CN_DYNAMIC_ID()                                             \
                                                name(type value)                                            \
                                                    : CrossNetRuntime::StructEnum<type, name>(value)       \
                                                {                                                           \
                                                }                                                           \
                                                name()                                                      \
                                                    : CrossNetRuntime::StructEnum<type, name>(0)           \
                                                {                                                           \
                                                }                                                           \
                                                operator type()                                             \
                                                {                                                           \
                                                    return (mValue);                                        \
                                                }                                                           \
                                                static void __CreateInterfaceMap__();                       \
                                                void __Trace__(unsigned char /* currentMark */)             \
                                                {                                                           \
                                                }

#define END_DECLARE_ENUM                    };

namespace System
{
    class String;
    class Array;
    class Object;
}

namespace CrossNetRuntime
{
    template <typename T, typename enumType>
    struct StructEnum
    {
/*
 * Don't do this here...
 * This creates an issue as the ID is then by size of enum and not type of enum...
 * Although it may not be true anymore as the struct is a template parameter as well...
 *
        CN_MULTIPLE_DYNAMIC_INTERFACE_ID0()
*/
        StructEnum()
            :
            mValue(0)
        {
        }

        StructEnum(T value)
            :
            mValue(value)
        {
            // Do nothing...
        }

        operator T()
        {
            return (mValue);
        }

        StructEnum * operator->()
        {
            return (this);
        }

        System::String *    ToString();

        System::Boolean     Equals(System::Object * obj)
        {
            // See if we can unbox it with the corresponding enum type
            BoxedObject<StructEnum<T, enumType> > * other =   AsCast<BoxedObject<StructEnum<T, enumType> > >(obj);
            if (other == NULL)
            {
                return (false);
            }
            return (mValue == other->get_Value().mValue);
        }

        System::Int32       GetHashCode()
        {
            return (System::Int32)(mValue);
        }

    protected:
        T   mValue;
    };
}

namespace System
{
    struct TypeCode;
    class IFormatProvider;

    template <typename T>
    class Array__G;

    struct Enum
    {
        CN_DYNAMIC_OBJECT_ID0(0)

        System::String * ToString(System::String *);
        System::String * ToString(System::String *, System::IFormatProvider *);
        System::String * ToString(System::IFormatProvider *);

        static System::String * GetName(System::Type *, System::Object *);
        static System::Type * GetUnderlyingType(System::Type *);
        static System::Array * GetValues(System::Type *);
        static System::Object * Parse(System::Type *, System::String *);
        static System::Object * Parse(System::Type *, System::String *, System::Boolean);
        ::System::TypeCode GetTypeCode();
        static System::String * Format(System::Type *, System::Object *, System::String * format);
        static System::Array__G<System::String *> * GetNames(System::Type *);
        static System::Boolean IsDefined(System::Type *, System::Object *);

        static System::Object * ToObject(System::Type *, System::SByte);
        static System::Object * ToObject(System::Type *, System::Int16);
        static System::Object * ToObject(System::Type *, System::Int32);
        static System::Object * ToObject(System::Type *, System::Int64);
        static System::Object * ToObject(System::Type *, System::Object *);
        static System::Object * ToObject(System::Type *, System::Byte);
        static System::Object * ToObject(System::Type *, System::UInt16);
        static System::Object * ToObject(System::Type *, System::UInt32);
        static System::Object * ToObject(System::Type *, System::UInt64);

        System::Int32 CompareTo(System::Object * target);

        // TODO:    See how we can get rid of this...
        Enum * operator->()
        {
            return (this);
        }
    };
}

#endif

