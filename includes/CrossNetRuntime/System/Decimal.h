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

#ifndef __SYSTEM_DECIMAL_H__
#define __SYSTEM_DECIMAL_H__

#include "CrossNetRuntime/Internal/Primitives.h"

namespace System
{
    template <typename T>
    class Array__G;

	// TODO: Implement those...
	struct Decimal
	{
/*
 *  The .Net version should not be needed here anymore - Need to move them in the corresponding BaseTypeWrapper
 *  Now that Decimal is considered as Predefined type, several accesses will be done by using the BaseTypeWrapper
 *  At least the methods...
 *  Decimal is now considered as predefined type in the parser for two reason:
 *      First, consistency with the other predefined types.
 *      Second, enables implicit / explicit conversion with the other predefined types...
 *
 */

        // We should not have to define the dynamic ID here, as it will be done by the wrapper...
        //CN_DYNAMIC_ID()

        Decimal();
        Decimal(::System::Double);
        Decimal(::System::Int32);
        Decimal(::System::Array__G< ::System::Int32 > *);
        Decimal(::System::Int64);
        Decimal(::System::Single);
        Decimal(::System::UInt32);
        Decimal(::System::UInt64);
        Decimal(::System::Int32, ::System::Int32, ::System::Int32, System::Boolean, System::Byte);

        void __ctor__(::System::Double);
        void __ctor__(::System::Int32);
        void __ctor__(::System::Array__G< ::System::Int32 > *);
        void __ctor__(::System::Int64);
        void __ctor__(::System::Single);
        void __ctor__(::System::UInt32);
        void __ctor__(::System::UInt64);
        void __ctor__(::System::Int32, ::System::Int32, ::System::Int32, System::Boolean, System::Byte);

        static System::Int32 op_Explicit__System__Int32(const System::Decimal & other);
        static System::Int64 op_Explicit__System__Int64(const System::Decimal & other);

        static ::System::Array__G< ::System::Int32 > * GetBits(const System::Decimal &);

        System::Boolean op_Equality(const Decimal & right)
        {
            return (mValue == right.mValue);
        }

        static System::Boolean op_Inequality(const Decimal & left, const Decimal & right)
        {
            return (left.mValue != right.mValue);
        }

        static System::Boolean op_Equality(const Decimal & left, const Decimal & right)
        {
            return (left.mValue == right.mValue);
        }

        static Decimal  op_Addition(const Decimal & left, const Decimal & right)
        {
            return (Decimal(left.mValue + right.mValue));
        }

        static Decimal  op_Modulus(const Decimal & left, const Decimal & right);

        static Decimal op_Increment(const Decimal & value)
        {
            Decimal d(value.mValue + 1.0f);
            return (d);
        }

        static Decimal op_Decrement(const Decimal & value)
        {
            Decimal d(value.mValue - 1.0f);
            return (d);
        }

        System::Boolean operator !=(const Decimal & other) const
        {
            return (mValue != other.mValue);
        }

        System::Boolean operator !=(const double other) const
        {
            return (mValue != other);
        }

        System::Boolean operator !=(const Int64 other) const
        {
            return (mValue != (double)other);
        }

        System::Boolean operator ==(const Decimal & other) const
        {
            return (mValue == other.mValue);
        }

        System::Boolean operator ==(const double other) const
        {
            return (mValue == other);
        }

        System::Boolean operator ==(const Int64 other) const
        {
            return (mValue == (double)other);
        }

        System::Decimal operator +(const Decimal & other) const
        {
            return (Decimal(mValue + other.mValue));
        }

        System::Decimal operator +(const double other) const
        {
            return (Decimal(mValue + other));
        }

        System::Decimal operator +(const int other) const
        {
            return (Decimal(mValue + (double)other));
        }

        System::Decimal operator -(const Decimal & other) const
        {
            return (Decimal(mValue - other.mValue));
        }

        System::Decimal operator -(const double other) const
        {
            return (Decimal(mValue - other));
        }

        System::Decimal operator -(const int other) const
        {
            return (Decimal(mValue - (double)other));
        }

        System::Decimal operator *(const Decimal & other) const
        {
            return (Decimal(mValue * other.mValue));
        }

        System::Decimal operator *(const double other) const
        {
            return (Decimal(mValue * other));
        }

        System::Decimal operator /(const Decimal & other) const
        {
            return (Decimal(mValue / other.mValue));
        }

        System::Decimal operator /(const double other) const
        {
            return (Decimal(mValue / other));
        }

        System::Boolean operator >(const Decimal & other) const
        {
            return (mValue > other.mValue);
        }

        System::Boolean operator >=(const Decimal & other) const
        {
            return (mValue >= other.mValue);
        }

        System::Boolean operator <(const Decimal & other) const
        {
            return (mValue < other.mValue);
        }

        System::Boolean operator <=(const Decimal & other) const
        {
            return (mValue <= other.mValue);
        }

        // This is used for GetHashCode()
        // Be careful as this operator should not be used in normal cases...
        operator System::Int32() const
        {
            int * ptr = (int *)(&mValue);
            // Read the double as integer xored
            return (ptr[0] ^ ptr[1]);
        }

        // In some cases, there are direct comparison between decimal and double
        operator double() const
        {
            return (mValue);
        }

        // This operator has been created to resolve a reflector Bug with decimal arrays...
        // TODO: Get rid of this when the bug is solved in Reflector
/*
 * this should be solved now...
 *
        Decimal & operator *()
        {
            return (*this);
        }
*/

        static System::Single op_Explicit__System__Single(const System::Decimal &);
        static System::Double op_Explicit__System__Double(const System::Decimal &);

//        static System::Decimal Truncate(System::Decimal);

    private:
        double  mValue;
        double  mPad;       // Add a padding so the size of decimal corresponds to the .Net version
                            // I.e. 128 bits - 16 bytes

        friend System::Decimal operator *(const Int32 left, const Decimal & right);
        friend System::Decimal operator *(const Int64 left, const Decimal & right);
        friend System::Decimal operator *(const double left, const Decimal & right);
	};

    CROSSNET_FINLINE
    System::Decimal operator *(const double left, const Decimal & right)
    {
        return (Decimal(left * right.mValue));
    }

    CROSSNET_FINLINE
    System::Decimal operator *(const Int32 left, const Decimal & right)
    {
        return (Decimal((double)left * right.mValue));
    }

    CROSSNET_FINLINE
    System::Decimal operator *(const Int64 left, const Decimal & right)
    {
        return (Decimal((double)left * right.mValue));
    }
}

#endif