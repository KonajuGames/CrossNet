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

#ifndef __POINTERWRAPPER_H__
#define __POINTERWRAPPER_H__

namespace CrossNetRuntime
{
    // Wrapper on the pointers (to disambiguate with ref / out)
    // We have to reimplement all the pointer operations.
    template <typename T>
    class PointerWrapper
    {
    public:
        PointerWrapper(T v)
            :
            mValue(v)
        {
            // Do nothing...
        }

        operator T()
        {
            return (mValue);
        }

        T ToValue()
        {
            return (mValue);
        }

        operator void *() const // Use const so it doesn't conflict with T = void *
        {
            return (void *)(mValue);
        }
/*
        operator intptr_t() const
        {
            return (intptr_t)(mValue);
        }
*/
        T   operator->()
        {
            return (mValue);
        }

        // Prefix
        PointerWrapper operator++()
        {
            ++mValue;
            return (*this);
        }

        PointerWrapper operator--()
        {
            --mValue;
            return (*this);
        }

        // Postfix
        PointerWrapper operator++(int /*dummy*/)
        {
            T   value = mValue;
            mValue++;
            return (value);
        }

        PointerWrapper operator--(int /*dummy*/)
        {
            T   value = mValue;
            mValue--;
            return (value);
        }

        PointerWrapper operator+(int offset)
        {
            T   value = mValue + offset;
            return (value);
        }

        PointerWrapper operator-(int offset)
        {
            T   value = mValue - offset;
            return (value);
        }

        ::System::Boolean operator==(PointerWrapper<T> & other)
        {
            return (mValue == other.mValue);
        }

        ::System::Boolean operator==(T other)
        {
            return (mValue == other);
        }
/*
        ::System::Boolean operator==(int other) const // Use const to differentiate if T == int
        {
            return ((int)mValue == other);
        }
*/
        ::System::Boolean operator!=(T other)
        {
            return (mValue != other);
        }
/*
        ::System::Boolean operator!=(int other) const // Use const to differentiate if T == int
        {
            return ((int)mValue != other);
        }
*/

        System::Int32 ToInt32()
        {
            return (System::Int32)(mValue);
        }

    private:
        T   mValue;
    };

    // Resolution of T doesn't work with templated class but does work with templated method...
    // So let's create this simple method so we don't have to determine the type during parsing...
    template <typename T>
    PointerWrapper<T> CreatePointerWrapper(T value)
    {
        return (PointerWrapper<T>(value));
    }

    template <typename T>
    CROSSNET_FINLINE
    ::System::Int32 PointerToInt32(CrossNetRuntime::PointerWrapper<T> p)
    {
        return (p.ToInt32());
    }

    CROSSNET_FINLINE
    ::System::Int32 PointerToInt32(void * p)
    {
        return (::System::Int32)(p);
    }

}

#endif

