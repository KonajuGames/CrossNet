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

#ifndef __CROSSNET_TRACE_H__
#define __CROSSNET_TRACE_H__

namespace CrossNetRuntime
{
    // There is a bit of a trick here...
    // As we don't know ahead of time the type of the array, we need traits to tell us what needs to be collected

    enum TraceMode
    {
        TM_NONE,
        TM_CLASS,
        TM_STRUCT
    };

    template <class T>
    struct GetTraceMode
    {
        static const TraceMode Value = TM_STRUCT;
    };

    template <class T>
    struct GetTraceMode<T *>        // If that's a pointer, consider it is a class
                                    //  Actually this is not correct for unsafe code
                                    //  But we are going to handle that in the GCManager class...
                                    // A better implementation would be to have a trait to detect if a type derives from System::Object
    {
        static const TraceMode Value = TM_CLASS;
    };

/*
    From some boost mailing lists - Rani Sharoni

        template <typename B, typename D>
        struct is_base_and_derived
        {
        private:
            typedef char (&yes)[1];
            typedef char (&no) [2];

            template<typename T>
            static yes check(D const volatile &, T);
            static no  check(B const volatile &, int);

            struct C
            {
                operator B const volatile &() const;
                operator D const volatile &();
            };

            static C getC();
        public:
            static const bool result =
                sizeof(check(getC(), 0)) == sizeof(yes);
        };
*/

#define CN_ISPRIMITIVE_TRAIT(Type)  \
    template <>                     \
    struct GetTraceMode<Type>       \
    {                               \
        static const TraceMode Value = TM_NONE; \
    };

    CN_ISPRIMITIVE_TRAIT(::System::Boolean)
    CN_ISPRIMITIVE_TRAIT(::System::Byte)
    CN_ISPRIMITIVE_TRAIT(::System::SByte)
    CN_ISPRIMITIVE_TRAIT(::System::Int16)
    CN_ISPRIMITIVE_TRAIT(::System::UInt16)
    CN_ISPRIMITIVE_TRAIT(::System::Char)
    CN_ISPRIMITIVE_TRAIT(::System::Int32)
    CN_ISPRIMITIVE_TRAIT(::System::UInt32)
    CN_ISPRIMITIVE_TRAIT(::System::Int64)
    CN_ISPRIMITIVE_TRAIT(::System::UInt64)
    CN_ISPRIMITIVE_TRAIT(::System::Single)
    CN_ISPRIMITIVE_TRAIT(::System::Double)
    CN_ISPRIMITIVE_TRAIT(::System::Decimal)

    template <typename U, typename TraceMode V>
    struct TraceTrait
    {
        static void DoTrace(unsigned char, U *, int)
        {
            // By default do nothing...
        }

        static void DoTrace(unsigned char, U &)
        {
            // By default do nothing...
        }
    };

    // Specialization for classes
    template <typename U>
    struct TraceTrait<U, TM_CLASS>
    {
        static void DoTrace(unsigned char currentMark, U * ptr, int size)
        {
            for (int i = 0 ; i < size ; ++i)
            {
                ::CrossNetRuntime::GCManager::Trace(ptr[i], currentMark);
            }
        }

        static void DoTrace(unsigned char currentMark, U ptr)
        {
            ::CrossNetRuntime::GCManager::Trace(ptr, currentMark);
        }
    };

    // Specialization for structs
    template <typename U>
    struct TraceTrait<U, TM_STRUCT>
    {
        static void DoTrace(unsigned char currentMark, U * ptr, int size)
        {
            for (int i = 0 ; i < size ; ++i)
            {
                ptr[i].__Trace__(currentMark);
            }
        }

        static void DoTrace(unsigned char currentMark, U & ptr)
        {
            ptr.__Trace__(currentMark);
        }
    };

    struct Tracer
    {
        template <typename U>
        static void DoTrace(unsigned char currentMark, U * ptr, int size)
        {
            TraceTrait<U, GetTraceMode<U>::Value >::DoTrace(currentMark, ptr, size);
        }

        template <typename U>
        static void DoTrace(unsigned char currentMark, U ptr)
        {
            TraceTrait<U, GetTraceMode<U>::Value >::DoTrace(currentMark, ptr);
        }
    };
}

#endif
