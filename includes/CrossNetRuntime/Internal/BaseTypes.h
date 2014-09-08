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

#ifndef __CROSSNET_BASE_TYPES_H__
#define __CROSSNET_BASE_TYPES_H__

#include <vector>

#include "CrossNetRuntime/Internal/EnumWrapper.h"
#include "CrossNetRuntime/Internal/Box.h"
#include "CrossNetRuntime/Internal/Cast.h"
#include "CrossNetRuntime/Internal/Tracer.h"

#include "CrossNetRuntime/System/IComparable.h"
#include "CrossNetRuntime/System/IComparable__G1.h"
#include "CrossNetRuntime/System/IFormattable.h"
#include "CrossNetRuntime/System/IEquatable__G1.h"
#include "CrossNetRuntime/System/Globalization/NumberStyles.h"
#include "CrossNetRuntime/System/Decimal.h"

#define CN_ARRAY_SIZE(t)   (sizeof(t) / sizeof(t[0]))

namespace System
{
    class Array;
    template <typename T>
    class Array__G;

    class String;
    class IFormatProvider;
    struct MidpointRounding;
    struct Currency;
    struct TypeCode;

    namespace Globalization
    {
        class CultureInfo;
        struct UnicodeCategory;
    }
}

namespace CrossNetRuntime
{
    // These classes are used to enables int.ToString(), etc...
    // They are also really important for generics as they hide the type specific issues

    template <typename T>
    class CommonBaseTypeWrapper
    {
    public:
        typedef T BoxeableType;

        CROSSNET_FINLINE
        CommonBaseTypeWrapper()
            :
            mValue()
        {
            // Do nothing...
        }

        CROSSNET_FINLINE
        CommonBaseTypeWrapper(T value)
            :
            mValue(value)
        {
            // Do nothing...
        }

        CROSSNET_FINLINE
        operator T()
        {
            return (mValue);
        }

        // It seems there are some cases where this is needed with generics
        // For example when we have to call Equals() on a generic type
        // TODO: Remove this and fix the issue...
        CommonBaseTypeWrapper<T> * operator->()
        {
            return (this);
        }

        // Declare but don't define the method, for two reasons:
        //  First, we need to specialize it by type
        //  Second, System::String is not defined yet, so we can't really use it...
        CROSSNET_FINLINE
        System::String * ToString();

        System::TypeCode GetTypeCode();

        CROSSNET_FINLINE
        System::Boolean  Equals(System::Object * obj)
        {
            BoxedObject<CommonBaseTypeWrapper> * other = AsCast<BoxedObject<CommonBaseTypeWrapper> >(obj);
            if (other == NULL)
            {
                return (false);
            }
            return (mValue == other->get_Value());
        }

        // A bit of trick here (yeah, again...)
        // Equals on the same type is defined so all the standard base type have access to this directly
        // The issue though is that in some case like T == System::Object *, Equals will actually be defined two times...
        // By adding the const as method modifier, we let the two methods cohabit in that particular case
        // Remember in the case where T = System::Object *, the wrapper doesn't do much, so there is no issue about having incorrect Equals() implementation
        // But at least, there won't be a compiler error...
        CROSSNET_FINLINE
        System::Boolean Equals(T obj) const
        {
            return (mValue == obj);
        }

//        static System::Boolean Equals(T left, T right);

        CROSSNET_FINLINE
        System::Int32    GetHashCode()
        {
            return (System::Int32)(mValue);
        }

        // Implements the code in the base class as it is pretty much the same on all types
        // (Except for the IFormattable function that we will implement correctly later...)

        // CompareTo should be implemented specifically for single, double and decimal though...
        CROSSNET_FINLINE
        System::Int32 CompareTo(System::Object * obj)
        {
            BoxedObject<CommonBaseTypeWrapper> * other = AsCast<BoxedObject<CommonBaseTypeWrapper> >(obj);
            if (other == NULL)
            {
                return (false);
            }
            return (System::Int32)(mValue - other->get_Value());
        }

        CROSSNET_FINLINE
        System::Int32 CompareToObj(System::Object * obj)
        {
            BoxedObject<CommonBaseTypeWrapper> * other = AsCast<BoxedObject<CommonBaseTypeWrapper> >(obj);
            if (other == NULL)
            {
                return (false);
            }
            return (System::Int32)(mValue - other->get_Value());
        }

        System::String * ToString(System::String * format, System::IFormatProvider * /*formatProvider*/)
        {
            CROSSNET_NOT_IMPLEMENTED();
            return (format);
        }

        // Same trick as earlier, it would conflict without the const for T = System::Object *
        // For example, this can happen for array of System.Object (set / get value are doing fake boxing / unboxing)
        // Thus forcing the instanciation of this whole class
        // If this still causes problem, we might be able to get away by specializing for System::Object *
        CROSSNET_FINLINE
        System::Int32 CompareTo(T other) const
        {
            return (System::Int32)(mValue - other);
        }

    protected:
        class Wrapper__IComparable : public ::System::IComparable
        {
        public:
            virtual System::Int32 CompareTo(void * __instance__, System::Object * obj)
            {
                BoxedObject<BaseTypeWrapper<T> > * temp = static_cast<BoxedObject<BaseTypeWrapper<T> > *>(__instance__);
                return (temp->get_Value().CompareToObj(obj));
            }
        };

        // Note char doesn't support IFormattable interface
        // Here we are breaking compatibility with .NET
        // TODO: Clean this...
        class Wrapper__IFormattable : public ::System::IFormattable
        {
        public:
            virtual System::String * ToString(void * __instance__, System::String * format, System::IFormatProvider * formatProvider)
            {
                BoxedObject<BaseTypeWrapper<T> > * temp = static_cast<BoxedObject<BaseTypeWrapper<T> > *>(__instance__);
                return (temp->get_Value().ToString(format, formatProvider));
            }
        };

        class Wrapper__IComparable__G1 : public ::System::IComparable__G1<T>
        {
        public:
            virtual System::Int32 CompareTo(void * __instance__, T other)
            {
                BoxedObject<BaseTypeWrapper<T> > * temp = static_cast<BoxedObject<BaseTypeWrapper<T> > *>(__instance__);
                return (temp->get_Value().CompareTo(other));
            }
        };

        class Wrapper__IEquatable__G1 : public ::System::IEquatable__G1<T>
        {
        public:
            virtual System::Boolean Equals(void * __instance__, T other)
            {
                BoxedObject<BaseTypeWrapper<T> > * temp = static_cast<BoxedObject<BaseTypeWrapper<T> > *>(__instance__);
                return (temp->get_Value().Equals(other));
            }
        };

        T mValue;
    };

    template <typename T>
    class BaseTypeWrapper : public CommonBaseTypeWrapper<T>
    {
    public:
/*
 * This code might not be needed...
 *
        CN_MULTIPLE_DYNAMIC_OBJECT_ID0(sizeof(BaseTypeWrapper), NULL)
*/
        CROSSNET_FINLINE
        BaseTypeWrapper(T value)
            :
            CommonBaseTypeWrapper(value)
        {
            // Do nothing...
        }

        CROSSNET_FINLINE
        operator T()
        {
            return (mValue);
        }

        CROSSNET_FINLINE
        static
        void * * __GetInterfaceMap__()
        {
            return (T::__GetInterfaceMap__());
        }
    };

    // Here are the specializations (for constants and methods).

    // Specialization for System::Int16
    template <>
    class BaseTypeWrapper<System::Int16> : public CommonBaseTypeWrapper<System::Int16>
    {
    public:
        CN_DYNAMIC_ID()
        typedef BaseTypeWrapper BoxeableType;

        BaseTypeWrapper(System::Int16 value)
            :
            CommonBaseTypeWrapper(value)
        {
            // Do nothing...
        }

        static const System::Int16 MaxValue = SHRT_MAX;
        static const System::Int16 MinValue = SHRT_MIN;

        System::String * ToString();

        System::String * ToString(System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString());
        }

        System::String * ToString(System::String * format);

        System::String * ToString(System::String * format, System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString(format));
        }

        static System::Int16 Parse(System::String * s);
        static System::Int16 Parse(System::IFormatProvider * /*provider*/);
        static System::Int16 Parse(System::String * format, System::Globalization::NumberStyles style);
        static System::Int16 Parse(System::String * format, System::IFormatProvider * /*provider*/);
        static System::Int16 Parse(System::String * format, System::Globalization::NumberStyles style, System::IFormatProvider * provider);
        static System::Boolean TryParse(System::String * s, System::Int16 * result);
        static System::Boolean TryParse(System::String * format, System::Globalization::NumberStyles style, System::IFormatProvider * provider, System::Int16 * result);
    };

    // Specialization for System::UInt16
    template <>
    class BaseTypeWrapper<System::UInt16> : public CommonBaseTypeWrapper<System::UInt16>
    {
    public:
        CN_DYNAMIC_ID()
        typedef BaseTypeWrapper BoxeableType;

        BaseTypeWrapper(System::UInt16 value)
            :
            CommonBaseTypeWrapper(value)
        {
            // Do nothing...
        }

        static const System::UInt16 MaxValue = 0xffff;
        static const System::UInt16 MinValue = 0x0000;

        System::String * ToString();

        System::String * ToString(System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString());
        }

        System::String * ToString(System::String * format);

        System::String * ToString(System::String * format, System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString(format));
        }

        static System::UInt16 Parse(System::String * s);
        static System::UInt16 Parse(System::IFormatProvider * /*provider*/);
        static System::UInt16 Parse(System::String * format, System::Globalization::NumberStyles style);
        static System::UInt16 Parse(System::String * format, System::IFormatProvider * /*provider*/);
        static System::UInt16 Parse(System::String * format, System::Globalization::NumberStyles style, System::IFormatProvider * provider);
    };

    // Specialization for System::Int32
    template <>
    class BaseTypeWrapper<System::Int32> : public CommonBaseTypeWrapper<System::Int32>
    {
    public:
        CN_DYNAMIC_ID()
        typedef BaseTypeWrapper BoxeableType;

        BaseTypeWrapper(System::Int32 value)
            :
            CommonBaseTypeWrapper(value)
        {
            // Do nothing...
        }

        static const System::Int32 MaxValue = 0x7fffffff;
        static const System::Int32 MinValue = (System::Int32)0x80000000;

        // Other ToString() implementations hide the base class ToString()
        // Using it will make base class ToString() reacheable again...
        using CommonBaseTypeWrapper::ToString;

        System::String * ToString(System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString());
        }

        System::String * ToString(System::String * format);

        System::String * ToString(System::String * format, System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString(format));
        }

        static System::Int32 Parse(System::String * s);
        static System::Int32 Parse(System::IFormatProvider * /*provider*/);
        static System::Int32 Parse(System::String * format, System::Globalization::NumberStyles style);
        static System::Int32 Parse(System::String * format, System::IFormatProvider * /*provider*/);
        static System::Int32 Parse(System::String * format, System::Globalization::NumberStyles style, System::IFormatProvider * provider);
    };

    // Specialization for System::UInt32
    template <>
    class BaseTypeWrapper<System::UInt32> : public CommonBaseTypeWrapper<System::UInt32>
    {
    public:
        CN_DYNAMIC_ID()
        typedef BaseTypeWrapper BoxeableType;

        BaseTypeWrapper(System::UInt32 value)
            :
            CommonBaseTypeWrapper(value)
        {
            // Do nothing...
        }

        static const System::UInt32 MaxValue = 0xffffffff;
        static const System::UInt32 MinValue = 0x00000000;

        System::String * ToString();

        System::String * ToString(System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString());
        }

        System::String * ToString(System::String * format);

        System::String * ToString(System::String * format, System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString(format));
        }

        static System::UInt32 Parse(System::String * s);
        static System::UInt32 Parse(System::IFormatProvider * /*provider*/);
        static System::UInt32 Parse(System::String * format, System::Globalization::NumberStyles style);
        static System::UInt32 Parse(System::String * format, System::IFormatProvider * /*provider*/);
        static System::UInt32 Parse(System::String * format, System::Globalization::NumberStyles style, System::IFormatProvider * provider);
    };

    // Specialization for System::Int64
    template <>
    class BaseTypeWrapper<System::Int64> : public CommonBaseTypeWrapper<System::Int64>
    {
    public:
        CN_DYNAMIC_ID()
        typedef BaseTypeWrapper BoxeableType;

        BaseTypeWrapper(System::Int64 value)
            :
            CommonBaseTypeWrapper(value)
        {
            // Do nothing...
        }

        static const System::Int64 MaxValue = 0x7fffffffffffffffL;
        static const System::Int64 MinValue = (System::Int64)0x8000000000000000L;

        System::String * ToString();

        System::String * ToString(System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString());
        }

        System::String * ToString(System::String * format);

        System::String * ToString(System::String * format, System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString(format));
        }

        static System::Int64 Parse(System::String * s);
        static System::Int64 Parse(System::IFormatProvider * /*provider*/);
        static System::Int64 Parse(System::String * format, System::Globalization::NumberStyles style);
        static System::Int64 Parse(System::String * format, System::IFormatProvider * /*provider*/);
        static System::Int64 Parse(System::String * format, System::Globalization::NumberStyles style, System::IFormatProvider * provider);
    };

    // Specialization for System::UInt64
    template <>
    class BaseTypeWrapper<System::UInt64> : public CommonBaseTypeWrapper<System::UInt64>
    {
    public:
        CN_DYNAMIC_ID()
        typedef BaseTypeWrapper BoxeableType;

        BaseTypeWrapper(System::UInt64 value)
            :
            CommonBaseTypeWrapper(value)
        {
            // Do nothing...
        }

        static const System::UInt64 MaxValue = 0xffffffffffffffffL;
        static const System::UInt64 MinValue = 0x0000000000000000L;

        System::String * ToString();

        System::String * ToString(System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString());
        }

        System::String * ToString(System::String * format);

        System::String * ToString(System::String * format, System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString(format));
        }

        static System::UInt64 Parse(System::String * s);
        static System::UInt64 Parse(System::IFormatProvider * /*provider*/);
        static System::UInt64 Parse(System::String * format, System::Globalization::NumberStyles style);
        static System::UInt64 Parse(System::String * format, System::IFormatProvider * /*provider*/);
        static System::UInt64 Parse(System::String * format, System::Globalization::NumberStyles style, System::IFormatProvider * provider);
    };

    // Specialization for System::Single
    template <>
    class BaseTypeWrapper<System::Single> : public CommonBaseTypeWrapper<System::Single>
    {
    public:
        CN_DYNAMIC_ID()
        typedef BaseTypeWrapper BoxeableType;

        BaseTypeWrapper(System::Single value)
            :
            CommonBaseTypeWrapper(value)
        {
            // Do nothing...
        }

        static const System::Single Epsilon;
        static const System::Single MaxValue;
        static const System::Single MinValue;
        static const System::Single NaN;
        static const System::Single NegativeInfinity;
        static const System::Single PositiveInfinity;

        System::String * ToString();

        System::String * ToString(System::String * /*format*/)
        {
            // Not supported...
            return (ToString());
        }

        System::String * ToString(System::IFormatProvider * /*provider*/);

        System::String * ToString(System::String * format, System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString(format));
        }

        static System::Single Parse(System::String * s);
        static System::Single Parse(System::IFormatProvider * /*provider*/);
        static System::Single Parse(System::String * format, System::Globalization::NumberStyles style);
        static System::Single Parse(System::String * format, System::IFormatProvider * /*provider*/);
        static System::Single Parse(System::String * format, System::Globalization::NumberStyles style, System::IFormatProvider * provider);

        static System::Boolean IsInfinity(System::Single);
        static System::Boolean IsNaN(System::Single);
        static System::Boolean IsPositiveInfinity(System::Single);
        static System::Boolean IsNegativeInfinity(System::Single);
        static System::Single Ceiling(System::Single);
    };

    // Specialization for System::Double
    template <>
    class BaseTypeWrapper<System::Double> : public CommonBaseTypeWrapper<System::Double>
    {
    public:
        CN_DYNAMIC_ID()
        typedef BaseTypeWrapper BoxeableType;

        BaseTypeWrapper(System::Double value)
            :
            CommonBaseTypeWrapper(value)
        {
            // Do nothing...
        }

        static const System::Double Epsilon;
        static const System::Double MaxValue;
        static const System::Double MinValue;
        static const System::Double NaN;
        static const System::Double NegativeInfinity;
        static const System::Double PositiveInfinity;
        static const System::Double NegativeZero;

        System::String * ToString();

        System::String * ToString(System::String * /*format*/)
        {
            // Not supported...
            return (ToString());
        }

        System::String * ToString(System::IFormatProvider * /*provider*/);

        System::String * ToString(System::String * format, System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString(format));
        }

        static System::Double Parse(System::String * s);
        static System::Double Parse(System::IFormatProvider * /*provider*/);
        static System::Double Parse(System::String * format, System::Globalization::NumberStyles style);
        static System::Double Parse(System::String * format, System::IFormatProvider * /*provider*/);
        static System::Double Parse(System::String * format, System::Globalization::NumberStyles style, System::IFormatProvider * provider);

        static System::Boolean TryParse(System::String *, System::Double *);
        static System::Boolean TryParse(System::String *, System::Globalization::NumberStyles style, System::IFormatProvider * provider, System::Double *);

        static System::Boolean IsInfinity(System::Double);
        static System::Boolean IsNegative(System::Double);
        static System::Boolean IsNaN(System::Double);
        static System::Boolean IsPositiveInfinity(System::Double);
        static System::Boolean IsNegativeInfinity(System::Double);
        static System::Double Ceiling(System::Double);
    };

    // Specialization for System::Decimal
    // BaseTypeWrapper should completely reproduce Decimal struct, this doesn't seem the right way to do
    // Might want to do something else instead
    template <>
    class BaseTypeWrapper<System::Decimal> : public CommonBaseTypeWrapper<System::Decimal>
    {
    public:
        CN_DYNAMIC_ID()
        typedef BaseTypeWrapper BoxeableType;

        using CommonBaseTypeWrapper::Equals;

        BaseTypeWrapper(const System::Decimal & value)
            :
            CommonBaseTypeWrapper(value)
        {
            // Do nothing...
        }

        static const System::Decimal MaxValue;
        static const System::Decimal MinValue;
        static const System::Decimal MinusOne;
        static const System::Decimal One;
        static const System::Decimal Zero;

        static System::Array__G< ::System::Int32 > * GetBits(const System::Decimal &);

        static System::Decimal Add(const System::Decimal & left, const System::Decimal & right);
        static System::Decimal Subtract(const System::Decimal & left, const System::Decimal & right);
        static System::Decimal Multiply(const System::Decimal & left, const System::Decimal & right);
        static System::Decimal Divide(const System::Decimal & left, const System::Decimal & right);
        static System::Decimal Remainder(const System::Decimal & left, const System::Decimal & right);
        static System::Boolean Equals(const System::Decimal & left, const System::Decimal & right);

        static System::Decimal op_Increment(const System::Decimal & value)
        {
            return (System::Decimal::op_Increment(value));
        }

        static System::Decimal Negate(const System::Decimal & value);

        static System::Decimal op_Decrement(const System::Decimal & value)
        {
            return (System::Decimal::op_Decrement(value));
        }

        static System::Decimal op_Modulus(const System::Decimal & left, const System::Decimal & right)
        {
            return (System::Decimal::op_Modulus(left, right));
        }

        static System::Decimal op_UnaryNegation(const System::Decimal & value);

        System::String * ToString();

        System::String * ToString(System::String * /*format*/)
        {
            // Not supported...
            return (ToString());
        }

        System::String * ToString(System::IFormatProvider * /*provider*/);

        System::String * ToString(System::String * format, System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString(format));
        }

        System::Boolean operator ==(const System::Decimal & other) const
        {
            return (mValue == other);
        }

        System::Boolean operator ==(const double other) const
        {
            return (mValue == other);
        }

        System::Boolean operator ==(const System::Int64 other) const
        {
            return (mValue == other);
        }

        System::Boolean operator !=(const System::Decimal & other) const
        {
            return (mValue != other);
        }

        System::Boolean operator !=(const double other) const
        {
            return (mValue != other);
        }

        System::Boolean operator !=(const System::Int64 other) const
        {
            return (mValue != other);
        }

        static void FCallAdd(System::Decimal *, System::Decimal, System::Decimal);
        static void FCallDivide(System::Decimal *, System::Decimal, System::Decimal);
        static void FCallFloor(System::Decimal *, System::Decimal);
        static void FCallMultiply(System::Decimal *, System::Decimal, System::Decimal);
        static void FCallRound(System::Decimal *, System::Decimal, System::Int32);
        static void FCallSubtract(System::Decimal *, System::Decimal, System::Decimal);
        static void FCallToCurrency(System::Currency *, System::Decimal);
        static System::Int32 FCallToInt32(System::Decimal);
        static void FCallTruncate(System::Decimal *, System::Decimal);

        static System::Int32 Compare(System::Decimal, System::Decimal);
        static System::Byte ToByte(System::Decimal);
        static System::Currency ToCurrency(System::Decimal);
        static System::Decimal ToDecimal(System::Array__G<System::Byte> *);
        static System::Double ToDouble(System::Decimal);
        static System::Int16 ToInt16(System::Decimal);
        static System::Int32 ToInt32(System::Decimal);
        static System::Int64 ToInt64(System::Decimal);
        static System::Int64 ToOACurrency(System::Decimal);
        static System::SByte ToSByte(System::Decimal);
        static System::Single ToSingle(System::Decimal);
        static System::UInt16 ToUInt16(System::Decimal);
        static System::UInt32 ToUInt32(System::Decimal);
        static System::UInt64 ToUInt64(System::Decimal);

        static System::Decimal Truncate(System::Decimal);

        static System::Decimal Parse(System::String * s);
        static System::Decimal Parse(System::IFormatProvider * /*provider*/);
        static System::Decimal Parse(System::String * format, System::Globalization::NumberStyles style);
        static System::Decimal Parse(System::String * format, System::IFormatProvider * /*provider*/);
        static System::Decimal Parse(System::String * format, System::Globalization::NumberStyles style, System::IFormatProvider * provider);
        static void GetBytes(System::Decimal d, System::Array__G<System::Byte> * buffer);

        static System::Boolean IsInfinity(System::Decimal);
        static System::Boolean IsNaN(System::Decimal);
        static System::Decimal Ceiling(System::Decimal);
        static System::Decimal Floor(System::Decimal);
        static System::Decimal Round(System::Decimal);
        static System::Decimal Round(System::Decimal, System::Int32);
        static System::Decimal Round(System::Decimal, System::MidpointRounding);
        static System::Decimal Round(System::Decimal, System::Int32, System::MidpointRounding);
        static System::Decimal Abs(System::Decimal);
        static System::Decimal Min(System::Decimal, System::Decimal);
        static System::Decimal Max(System::Decimal, System::Decimal);
    };

    // Specialization for System::Boolean
    template <>
    class BaseTypeWrapper<System::Boolean> : public CommonBaseTypeWrapper<System::Boolean>
    {
    public:
        CN_DYNAMIC_ID()
        typedef BaseTypeWrapper BoxeableType;

        BaseTypeWrapper(System::Boolean value)
            :
            CommonBaseTypeWrapper(value)
        {
            // Do nothing...
        }

        CROSSNET_FINLINE
        System::Int32 CompareTo(System::Boolean other) const
        {
            if (mValue == other)
            {
                return (0);
            }
            if (mValue == false)
            {
                return (-1);
            }
            return (1);
        }

        static System::String * TrueString;
        static System::String * FalseString;

        System::String * ToString();

        System::String * ToString(System::IFormatProvider *);

        static System::Boolean  Parse(System::String * s);
        static System::Boolean Parse(System::IFormatProvider * /*provider*/);
        static System::Boolean Parse(System::String * format, System::Globalization::NumberStyles style);
        static System::Boolean Parse(System::String * format, System::IFormatProvider * /*provider*/);
        static System::Boolean Parse(System::String * format, System::Globalization::NumberStyles style, System::IFormatProvider * provider);
    };

    // Specialization for System::Char
    template <>
    class BaseTypeWrapper<System::Char> : public CommonBaseTypeWrapper<System::Char>
    {
    public:
        CN_DYNAMIC_ID()
        typedef BaseTypeWrapper BoxeableType;

        BaseTypeWrapper(System::Char value)
            :
            CommonBaseTypeWrapper(value)
        {
            // Do nothing...
        }

        System::String * ToString();
        static System::String * ToString(System::Char);
        System::String * ToString(System::IFormatProvider * /*format*/)
        {
            // Not supported...
            return (ToString());
        }

        // This is not part of the .NET API, but the BaseTypeWrapper implementation implements
        // IFormattable, we'll have to fix this later
        // TODO: Fix this
        System::String * ToString(System::String * format, System::IFormatProvider * /*provider*/);

        static System::Char Parse(System::String * s);
        static System::Char Parse(System::IFormatProvider * /*provider*/);
        static System::Char Parse(System::String * format, System::Globalization::NumberStyles style);
        static System::Char Parse(System::String * format, System::IFormatProvider * /*provider*/);
        static System::Char Parse(System::String * format, System::Globalization::NumberStyles style, System::IFormatProvider * provider);

        static System::Char ToUpper(System::Char);
        static System::Char ToUpper(System::Char, System::Globalization::CultureInfo *);
        static System::Char ToLower(System::Char);
        static System::Char ToLower(System::Char, System::Globalization::CultureInfo *);
        static System::Boolean IsDigit(System::Char);
        static System::Boolean IsDigit(System::String * s, System::Int32 index);
        static System::Boolean IsLetter(System::Char);
        static System::Boolean IsLetter(System::String * s, System::Int32 index);
        static System::Boolean IsLetterOrDigit(System::Char);
        static System::Boolean IsLetterOrDigit(System::String * s, System::Int32 index);
        static System::Boolean IsSeparator(System::Char);
        static System::Boolean IsSeparator(System::String * s, System::Int32 index);
        static System::Boolean IsSymbol(System::Char);
        static System::Boolean IsSymbol(System::String * s, System::Int32 index);
        static System::Boolean IsControl(System::Char);
        static System::Boolean IsControl(System::String * s, System::Int32 index);
        static System::Boolean IsPunctuation(System::Char);
        static System::Boolean IsPunctuation(System::String * s, System::Int32 index);
        static System::Boolean IsLower(System::Char);
        static System::Boolean IsLower(System::String * s, System::Int32 index);
        static System::Boolean IsUpper(System::Char);
        static System::Boolean IsUpper(System::String * s, System::Int32 index);
        static System::Boolean IsNumber(System::Char);
        static System::Boolean IsNumber(System::String * s, System::Int32 index);
        static System::Boolean IsWhiteSpace(System::Char);
        static System::Boolean IsWhiteSpace(System::String * s, System::Int32 index);
        static System::Boolean IsLowSurrogate(System::Char);
        static System::Boolean IsLowSurrogate(System::String * s, System::Int32 index);
        static System::Boolean IsHighSurrogate(System::Char);
        static System::Boolean IsHighSurrogate(System::String * s, System::Int32 index);
        static System::Boolean IsSurrogate(System::Char);
        static System::Boolean IsSurrogate(System::String * s, System::Int32 index);
        static System::Globalization::UnicodeCategory GetUnicodeCategory(System::Char);
        static System::Globalization::UnicodeCategory GetUnicodeCategory(System::String * s, System::Int32 index);
        static System::Boolean IsSurrogatePair(System::Char, System::Char);
        static System::Boolean IsSurrogatePair(System::String * s, System::Int32 index);
        static System::String * ConvertFromUtf32(System::Int32 utf32);
        static System::Int32 ConvertToUtf32(System::Char highSurrogate, System::Char lowSurrogate);
        static System::Int32 ConvertToUtf32(System::String * s, System::Int32 index);
        static System::Double GetNumericValue(System::Char);
        static System::Double GetNumericValue(System::String * s, System::Int32 index);
    };

    // Specialization for System::Byte
    template <>
    class BaseTypeWrapper<System::Byte> : public CommonBaseTypeWrapper<System::Byte>
    {
    public:
        CN_DYNAMIC_ID()
        typedef BaseTypeWrapper BoxeableType;

        BaseTypeWrapper(System::Byte value)
            :
            CommonBaseTypeWrapper(value)
        {
            // Do nothing...
        }

        static const System::Byte MaxValue = 0xff;
        static const System::Byte MinValue = 0x00;

        System::String * ToString();

        System::String * ToString(System::String * /*format*/)
        {
            // Not supported...
            return (ToString());
        }

        System::String * ToString(System::IFormatProvider * /*provider*/);

        System::String * ToString(System::String * format, System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString(format));
        }

        static System::Byte Parse(System::String * s);
        static System::Byte Parse(System::IFormatProvider * /*provider*/);
        static System::Byte Parse(System::String * format, System::Globalization::NumberStyles style);
        static System::Byte Parse(System::String * format, System::IFormatProvider * /*provider*/);
        static System::Byte Parse(System::String * format, System::Globalization::NumberStyles style, System::IFormatProvider * provider);
    };

    // Specialization for System::SByte
    template <>
    class BaseTypeWrapper<System::SByte> : public CommonBaseTypeWrapper<System::SByte>
    {
    public:
        CN_DYNAMIC_ID()
        typedef BaseTypeWrapper BoxeableType;

        BaseTypeWrapper(System::SByte value)
            :
            CommonBaseTypeWrapper(value)
        {
            // Do nothing...
        }

        static const System::SByte MaxValue = 0x7f;
        static const System::SByte MinValue = -128; // Put decimal number with sign to avoid out of range warning

        System::String * ToString();

        System::String * ToString(System::String * /*format*/)
        {
            // Not supported...
            return (ToString());
        }

        System::String * ToString(System::IFormatProvider * /*provider*/);

        System::String * ToString(System::String * format, System::IFormatProvider * /*provider*/)
        {
            // Not supported...
            return (ToString(format));
        }

        static System::SByte Parse(System::String * s);
        static System::SByte Parse(System::IFormatProvider * /*provider*/);
        static System::SByte Parse(System::String * format, System::Globalization::NumberStyles style);
        static System::SByte Parse(System::String * format, System::IFormatProvider * /*provider*/);
        static System::SByte Parse(System::String * format, System::Globalization::NumberStyles style, System::IFormatProvider * provider);
    };

// Use a macro for the wrapper, in reality what we would like is define a Wrapper type
// And specialize it for classes deriving from System::Object and struct
// For the moment I don't see any easy way and this method is simple and works anyway on any compiler
// I'm sure there is something more integrated that we could do...

    // Default template for all the types...
    // We use the class, for specialization by type when it's known at parsing time
    // This is mainly used during boxing (as we have to create the boxed class with the wrapper type).
    template <typename T>
    class GenWrapper
    {
    public:
        typedef T ConvertedType;
        // For the struct
        // If the template parameter doesn't work with the compiler, the user has to override the operator -> to achieve the same result
        // The annoying part is every single struct will have to override it
        static T & Convert(T & v)
        {
            return (v);
        }

        // For the classes
        static T * Convert(T * v)
        {
            return (v);
        }
    };

    // Default template for all the type
    // We use the function when we don't know at parsing time the type, this is more to bind losely coupled generics
    // For structs
    template <typename T>
    T & GenWrapperConvert(T & v)
    {
        return (v);
    }

    // For classes
    template <typename T>
    T * GenWrapperConvert(T * v)
    {
        return (v);
    }

    // For enums
    CROSSNET_FINLINE
    System::Enum GenWrapperConvert(System::Enum v)
    {
        return (v);
    }

#define DEFINE_WRAPPER(t)                               \
    template <>                                         \
    class GenWrapper<t>                                 \
    {                                                   \
    public:                                             \
        typedef BaseTypeWrapper<t> ConvertedType;       \
                                                        \
        BaseTypeWrapper<t> Convert(t v)                 \
        {                                               \
            return (BaseTypeWrapper<t>(v));             \
        }                                               \
    };                                                  \
                                                        \
    CROSSNET_FINLINE                                    \
    BaseTypeWrapper<t> GenWrapperConvert(t v)           \
    {                                                   \
        return (BaseTypeWrapper<t>(v));                 \
    }

    // And define the specialized version for all the other types

    DEFINE_WRAPPER(System::Boolean)
	DEFINE_WRAPPER(System::Byte)
	DEFINE_WRAPPER(System::SByte)
	DEFINE_WRAPPER(System::Char)
	DEFINE_WRAPPER(System::UInt16)
	DEFINE_WRAPPER(System::Int16)
	DEFINE_WRAPPER(System::UInt32)
    DEFINE_WRAPPER(System::Int32)
	DEFINE_WRAPPER(System::Int64)
	DEFINE_WRAPPER(System::UInt64)
	DEFINE_WRAPPER(System::Single)
	DEFINE_WRAPPER(System::Double)
	DEFINE_WRAPPER(System::Decimal)

#undef DEFINE_WRAPPER

    // Same kind of thing here, but this to determine the default value
    // It's either NULL for a class, or the default constructor for a struct (0 for a base type).

    template <typename T>
    T   GenDefault(T * /* dummy */)
    {
        return (T());
    }

    template <typename T>
    typename T * GenDefault(System::Object * /* dummy */)
    {
        return (NULL);
    }

    template <typename U, typename TraceMode V>
    struct ParameterlessConstructTrait
    {
        static U Construct()
        {
            return (0);
        }
    };

    // Specialization for class
    template <typename U>
    struct ParameterlessConstructTrait<U, TM_CLASS>
    {
        static U Construct()
        {
        return (System::Activator::CreateInstance__G1<U>());
        }
    };

    // Specialization for struct
    template <typename U>
    struct ParameterlessConstructTrait<U, TM_STRUCT>
    {
        static U Construct()
        {
            return (U());
        }
    };

    // For parameter less construct on values, use the gen default implementation
    template <typename T>
    T   ParameterlessConstruct(const T & /* dummy */)
    {
        return (GenDefault<T>((T *)NULL));
    }

    // For parameter less construct on objects, use the activator
    template <typename T>
    typename T ParameterlessConstruct(System::Object * /* dummy */)
    {
        return (System::Activator::CreateInstance__G1<T>());
    }

#define DEFINE_DEFAULT(T, value)                \
    CROSSNET_FINLINE                            \
    T GenDefault(T * /* dummy */)               \
    {                                           \
        return (value);                         \
    }

    DEFINE_DEFAULT(System::Boolean, false)
	DEFINE_DEFAULT(System::Byte, 0)
	DEFINE_DEFAULT(System::SByte, 0)
	DEFINE_DEFAULT(System::Char, 0)
	DEFINE_DEFAULT(System::UInt16, 0)
	DEFINE_DEFAULT(System::Int16, 0)
	DEFINE_DEFAULT(System::UInt32, 0)
    DEFINE_DEFAULT(System::Int32, 0)
	DEFINE_DEFAULT(System::Int64, 0)
	DEFINE_DEFAULT(System::UInt64, 0)
	DEFINE_DEFAULT(System::Single, 0.0f)
	DEFINE_DEFAULT(System::Double, 0.0f)
	DEFINE_DEFAULT(System::Decimal, 0.0f)

#define __DEFAULT__(type)           CrossNetRuntime::GenDefault<type>((type *)NULL)
//#define __PARAMETERLESS_NEW__(type) CrossNetRuntime::ParameterlessConstruct<type>((type)NULL)
#define __PARAMETERLESS_NEW__(type) CrossNetRuntime::ParameterlessConstructTrait<type, CrossNetRuntime::GetTraceMode<type>::Value>::Construct()

template <typename T, typename enumName>
CROSSNET_FINLINE
System::String *    StructEnum<T, enumName>::ToString()
{
    return (System::String::__Create__(L"Enum"));
}

}

#endif	//	__CROSSNET_BASE_TYPES_H__

