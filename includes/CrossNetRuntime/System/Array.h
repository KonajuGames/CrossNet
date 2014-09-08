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

#ifndef __CROSSNET_ARRAY_H__
#define __CROSSNET_ARRAY_H__

#include "CrossNetRuntime/System/ICloneable.h"
#include "CrossNetRuntime/System/Collections/IList.h"
#include "CrossNetRuntime/System/Collections/ICollection.h"
#include "CrossNetRuntime/System/Collections/IEnumerator.h"
#include "CrossNetRuntime/System/Collections/IEnumerable.h"
#include "CrossNetRuntime/System/Collections/Generic/IEnumerator__G1.h"
#include "CrossNetRuntime/System/Collections/Generic/IEnumerable__G1.h"
#include "CrossNetRuntime/System/Collections/Generic/IList__G1.h"
#include "CrossNetRuntime/System/Collections/Generic/ICollection__G1.h"
#include "CrossNetRuntime/GC/GCManager.h"
#include "CrossNetRuntime/Internal/Tracer.h"
#include "CrossNetRuntime/Internal/Typeof.h"

namespace System
{
    struct Decimal;
}

namespace System
{
    namespace Collections
    {
        class IComparer;

        namespace ObjectModel
        {
            template <typename T>
            class ReadOnlyCollection;
        }

        namespace Generic
        {
            template <typename T>
            class IComparer__G1;
        }
    }

    template <typename T>
    class Comparison__G1;

    template <typename T>
    class Array__G;

    // Abstract array class - base class for all arrays
    class Array : public System::Object
    {
    public:
        CN_DYNAMIC_ID();

        virtual void SetValue(System::Object * value, System::Int32 first) = 0;
        virtual void SetValue(System::Object * value, System::Array__G<System::Int32> *) = 0;
        virtual void SetValue(System::Object * value, System::Int64 first) = 0;
        virtual void SetValue(System::Object * value, System::Array__G<System::Int64> *) = 0;
        virtual void SetValue(System::Object * value, System::Int32 first, System::Int32 second) = 0;
        virtual void SetValue(System::Object * value, System::Int64 first, System::Int64 second) = 0;
        virtual void SetValue(System::Object * value, System::Int32 first, System::Int32 second, System::Int32 third) = 0;
        virtual void SetValue(System::Object * value, System::Int64 first, System::Int64 second, System::Int64 third) = 0;

        virtual System::Object * GetValue(System::Int32 first) = 0;
        virtual System::Object * GetValue(System::Array__G<System::Int32> *) = 0;
        virtual System::Object * GetValue(System::Int64 first) = 0;
        virtual System::Object * GetValue(System::Array__G<System::Int64> *) = 0;
        virtual System::Object * GetValue(System::Int32 first, System::Int32 second) = 0;
        virtual System::Object * GetValue(System::Int64 first, System::Int64 second) = 0;
        virtual System::Object * GetValue(System::Int32 first, System::Int32 second, System::Int32 third) = 0;
        virtual System::Object * GetValue(System::Int64 first, System::Int64 second, System::Int64 third) = 0;

        virtual int get_Length() const = 0;
        virtual Int32 GetLength(Int32 dimension) = 0;
        virtual Int32 get_Rank() = 0;
        virtual Int32 GetLowerBound(Int32 dimension) = 0;
        virtual Int32 GetUpperBound(Int32 dimension) = 0;

        // So we can do some type safety, at the item level
        virtual void * * GetItemInterfaceMap() = 0;

        Boolean get_IsFixedSize()
        {
            return (true);
        }

        Boolean get_IsReadOnly()
        {
            return (false);
        }

        Int32 IList__Add(System::Object * /*value*/)
        {
            CROSSNET_DONT_CALL();
            return (-1);
        }

        void IList__Clear()
        {
            CROSSNET_DONT_CALL();
        }

        // Use const for these versions so they don't collide with generic version
        System::Boolean Contains(System::Object * value) const
        {
            return (IndexOf(value) >= 0);
        }

        // Use const for these versions so they don't collide with generic version
        virtual System::Int32 IndexOf(System::Object * value) const = 0;

        void IList__Insert(System::Int32 /*index*/, System::Object * /*value*/)
        {
            CROSSNET_DONT_CALL();
        }

        void IList__Remove(System::Object * /*value*/)
        {
            CROSSNET_DONT_CALL();
        }

        void IList__RemoveAt(System::Int32 /*index*/)
        {
            CROSSNET_DONT_CALL();
        }

        Boolean get_IsSynchronized()
        {
            return (false);
        }

        System::Object * get_SyncRoot()
        {
            return (this);
        }

        System::Object * Clone();

        void CopyTo(System::Array * array, System::Int32 index)
        {
            // Make sure that both arrays are of the same type
            CROSSNET_ASSERT(m__InterfaceMap__ == array->m__InterfaceMap__, "");

            int sizeOfT = GetSizeOfT();
            int length = get_Length();

            // Make sure that the destination array is long enough
            CROSSNET_ASSERT(length + index <= array->get_Length(), "");

            void * arraySrcItems = GetAddressOfFirstItem();
            void * arrayDstItems = (void *)((int)(array->GetAddressOfFirstItem()) + (index * sizeOfT));

            __memcopy__(arrayDstItems, arraySrcItems, length * sizeOfT);
        }

        static void Copy(System::Array *, System::Array *, int);
        static void Copy(System::Array *, System::Array *, long);
        static void Copy(System::Array *, int, System::Array *, int, int);
        static void Copy(System::Array *, long, System::Array *, long, long);
        static void Clear(System::Array *, System::Int32, System::Int32);

        static void Reverse(System::Array * array)
        {
            array->Reverse();
        }
        static void Reverse(System::Array *, System::Int32, System::Int32);

        // Dynamic creation of the type - don't expect this to be implemented soon
        // I believe this could be implemented by hacking things around,
        // Like by getting the size of the corresponding size, allocating correctly, clearing up the buffer
        // (as if the type is a class, it contains null references, if that's a struct, all the members are null,
        // it could be different for enums though - value of the first enum?)
        // We would have to see how the Id would be determined...
        static System::Array * CreateInstance(System::Type *, System::Int32);
        static System::Array * CreateInstance(System::Type *, System::Array__G<System::Int32> *);
        static System::Array * CreateInstance(System::Type *, System::Array__G<System::Int64> *);
        static System::Array * CreateInstance(System::Type *, System::Int32, System::Int32);
        static System::Array * CreateInstance(System::Type *, System::Array__G<System::Int32> *, System::Array__G<System::Int32> *);
        static System::Array * CreateInstance(System::Type *, System::Int32, System::Int32, System::Int32);

        static System::Int32 IndexOf(System::Array *, System::Object *);
        static System::Int32 IndexOf(System::Array *, System::Object *, System::Int32);
        static System::Int32 IndexOf(System::Array *, System::Object *, System::Int32, System::Int32);
        template <typename T>
        static System::Int32 IndexOf__G1(System::Array__G<T> *, T);
        template <typename T>
        static System::Int32 IndexOf__G1(System::Array__G<T> *, T, System::Int32);
        template <typename T>
        static System::Int32 IndexOf__G1(System::Array__G<T> *, T, System::Int32, System::Int32);

        static System::Int32 LastIndexOf(System::Array *, System::Object *);
        static System::Int32 LastIndexOf(System::Array *, System::Object *, System::Int32);
        static System::Int32 LastIndexOf(System::Array *, System::Object *, System::Int32, System::Int32);
        template <typename T>
        static System::Int32 LastIndexOf__G1(System::Array__G<T> *, T);
        template <typename T>
        static System::Int32 LastIndexOf__G1(System::Array__G<T> *, T, System::Int32);
        template <typename T>
        static System::Int32 LastIndexOf__G1(System::Array__G<T> *, T, System::Int32, System::Int32);

        template <typename T>
        static System::Collections::ObjectModel::ReadOnlyCollection<T> * AsReadOnly__G1(System::Array__G<T> *);

        static System::Int32 BinarySearch(System::Array *, System::Object *);
        static System::Int32 BinarySearch(System::Array *, System::Object *, System::Collections::IComparer *);
        static System::Int32 BinarySearch(System::Array *, System::Int32, System::Int32, System::Object *);
        static System::Int32 BinarySearch(System::Array *, System::Int32, System::Int32, System::Object *, System::Collections::IComparer *);

        template <typename T>
        static System::Int32 BinarySearch__G1(System::Array__G<T> *, T);
        template <typename T>
        static System::Int32 BinarySearch__G1(System::Array__G<T> *, T, System::Collections::Generic::IComparer__G1<T> *);
        template <typename T>
        static System::Int32 BinarySearch__G1(System::Array__G<T> *, System::Int32, System::Int32, T);
        template <typename T>
        static System::Int32 BinarySearch__G1(System::Array__G<T> *, System::Int32, System::Int32, T, System::Collections::Generic::IComparer__G1<T> *);

        static void Sort(System::Array *);
        static void Sort(System::Array *, System::Array *);
        static void Sort(System::Array *, System::Collections::IComparer *);
        static void Sort(System::Array *, System::Array *, System::Collections::IComparer *);

        static void Sort(System::Array *, System::Int32, System::Int32);
        static void Sort(System::Array *, System::Array *, System::Int32, System::Int32);
        static void Sort(System::Array *, System::Int32, System::Int32, System::Collections::IComparer *);
        static void Sort(System::Array *, System::Array *, System::Int32, System::Int32, System::Collections::IComparer *);

        template <typename T>
        static void Sort__G1(System::Array__G<T> *)
        {
            CROSSNET_NOT_IMPLEMENTED();
        }
        template <typename T>
        static void Sort__G1(System::Array__G<T> *, Comparison__G1<T> *);
        template <typename T>
        static void Sort__G1(System::Array__G<T> *, T, System::Collections::Generic::IComparer__G1<T> *);
        template <typename T>
        static void Sort__G1(System::Array__G<T> *, System::Int32, System::Int32);
        template <typename T>
        static void Sort__G1(System::Array__G<T> *, System::Int32, System::Int32, System::Collections::Generic::IComparer__G1<T> *);

        template <typename TKey, typename TValue>
        static void Sort__G2(System::Array__G<TKey> *, System::Array__G<TValue> *);
        template <typename TKey, typename TValue>
        static void Sort__G2(System::Array__G<TKey> *, System::Array__G<TValue> *, System::Collections::Generic::IComparer__G1<TKey> *);
        template <typename TKey, typename TValue>
        static void Sort__G2(System::Array__G<TKey> *, System::Array__G<TValue> *, System::Int32, System::Int32);
        template <typename TKey, typename TValue>
        static void Sort__G2(System::Array__G<TKey> *, System::Array__G<TValue> *, System::Int32, System::Int32, System::Collections::Generic::IComparer__G1<TKey> *);

    protected:
        CROSSNET_FINLINE
        Array()
            :   Object(__ARRAY__)           // Tells that's a dynamically sized object
        {
            // Do nothing...
        }

        CROSSNET_FINLINE
        ~Array()
        {
            // Do nothing...
        }

        virtual void Reverse() = 0;
        virtual int GetSizeOfT() = 0;
        virtual void * GetAddressOfFirstItem() = 0;

    private:
        Array(const Array & other);
        Array & operator=(const Array & other);
    };

    template<class T>
    class Array__G : public Array
    {
    public:
        CN_MULTIPLE_DYNAMIC_OBJECT_ID
        (
            sizeof(Array__G),
            __W7__
            (
                CN_IMPLEMENT(Wrapper__ICloneable),
                CN_IMPLEMENT(Wrapper__IList),
                CN_IMPLEMENT(Wrapper__ICollection),
                CN_IMPLEMENT(Wrapper__IEnumerable),
                CN_IMPLEMENT(Wrapper__IList__G1),
                CN_IMPLEMENT(Wrapper__ICollection__G1),
                CN_IMPLEMENT(Wrapper__IEnumerable__G1)
            )
            ,
            NULL
        )

        static Array__G * __Create__(int first, T * initValues = NULL)
        {
            Array__G * array = (Array__G *)operator new(sizeof(Array__G) + (sizeof(T) * first));
            array->Array__G::Array__G(first, initValues);
            return (array);
        }

        static Array__G * __Create__(int first, int second, T * initValues = NULL)
        {
            Array__G * array = (Array__G *)operator new(sizeof(Array__G) + (sizeof(T) * first * second));
            array->Array__G::Array__G(first, second, initValues);
            return (array);
        }

        static Array__G * __Create__(int first, int second, int third, T * initValues = NULL)
        {
            Array__G * array = (Array__G *)operator new(sizeof(Array__G) + (sizeof(T) * first * second * third));
            array->Array__G::Array__G(first, second, third, initValues);
            return (array);
        }

        static Array__G * __Create__(int first, int second, int third, int fourth, T * initValues = NULL);
        static Array__G * __Create__(int first, int second, int third, int fourth, int fifth, T * initValues = NULL);
        static Array__G * __Create__(int first, int second, int third, int fourth, int fifth, int sixth, T * initValues = NULL);

        virtual int __GetVariableSize__()
        {
            int size = sizeof(Array__G);
            size += sizeof(T) * GetSize();
            return (size);
        }

        T * __ToPointer__()
        {
            return (mItems);
        }

        const T & Item(int first) const
        {
            CROSSNET_ASSERT(mSecond == 0, "This is not a one dimension array!");
            CROSSNET_ASSERT(((first >= 0) && (first < mFirst)), "Out of bound!");
            int index = first;
            return (*(mItems + index));
        }

        T & Item(int first)
        {
            CROSSNET_ASSERT(mSecond == 0, "This is not a one dimension array!");
            CROSSNET_ASSERT(((first >= 0) && (first < mFirst)), "Out of bound!");
            int index = first;
            return (*(mItems + index));
        }

        const T & Item(int first, int second) const
        {
            CROSSNET_ASSERT(mThird == 0, "This is not a two dimensions array!");
            CROSSNET_ASSERT(((first >= 0) && (first < mFirst)), "Out of bound!");
            CROSSNET_ASSERT(((second >= 0) && (second < mSecond)), "Out of bound!");
            int index = (first * mSecond) + second;
            return (*(mItems + index));
        }

        T & Item(int first, int second)
        {
            CROSSNET_ASSERT(mThird == 0, "This is not a two dimensions array!");
            CROSSNET_ASSERT(((first >= 0) && (first < mFirst)), "Out of bound!");
            CROSSNET_ASSERT(((second >= 0) && (second < mSecond)), "Out of bound!");
            int index = (first * mSecond) + second;
            return (*(mItems + index));
        }

        const T & Item(int first, int second, int third) const
        {
            CROSSNET_ASSERT(mThird != 0, "This is not a three dimensions array!");
            CROSSNET_ASSERT(((first >= 0) && (first < mFirst)), "Out of bound!");
            CROSSNET_ASSERT(((second >= 0) && (second < mSecond)), "Out of bound!");
            CROSSNET_ASSERT(((third >= 0) && (third < mThird)), "Out of bound!");
            int index = (((first * mSecond) + second) * mThird) + third;
            return (*(mItems + index));
        }

        T & Item(int first, int second, int third)
        {
            CROSSNET_ASSERT(mThird != 0, "This is not a three dimensions array!");
            CROSSNET_ASSERT(((first >= 0) && (first < mFirst)), "Out of bound!");
            CROSSNET_ASSERT(((second >= 0) && (second < mSecond)), "Out of bound!");
            CROSSNET_ASSERT(((third >= 0) && (third < mThird)), "Out of bound!");
            int index = (((first * mSecond) + second) * mThird) + third;
            return (*(mItems + index));
        }

        const T & SingleDimensionItem(int first) const
        {
            CROSSNET_ASSERT(((first >= 0) && (first < GetSize())), "Out of bound!");
            int index = first;
            return (*(mItems + index));
        }

        T & SingleDimensionItem(int first)
        {
            CROSSNET_ASSERT(((first >= 0) && (first < GetSize())), "Out of bound!");
            int index = first;
            return (*(mItems + index));
        }

        void SetValue(System::Object * value, System::Int32 first)
        {
            T temp = CrossNetRuntime::Unbox<CrossNetRuntime::BaseTypeWrapper<T>::BoxeableType >(value);
            Item(first) = temp;
        }

        virtual void SetValue(System::Object *, System::Array__G<System::Int32> *)
        {
            CROSSNET_NOT_IMPLEMENTED();
        }

        virtual void SetValue(System::Object * value, System::Int64 first)
        {
            T temp = CrossNetRuntime::Unbox<CrossNetRuntime::BaseTypeWrapper<T>::BoxeableType >(value);
            Item((int)first) = temp;
        }

        virtual void SetValue(System::Object *, System::Array__G<System::Int64> *)
        {
            CROSSNET_NOT_IMPLEMENTED();
        }

        virtual void SetValue(System::Object * value, System::Int32 first, System::Int32 second)
        {
            T temp = CrossNetRuntime::Unbox<CrossNetRuntime::BaseTypeWrapper<T>::BoxeableType >(value);
            Item(first, second) = temp;
        }

        virtual void SetValue(System::Object * value, System::Int64 first, System::Int64 second)
        {
            T temp = CrossNetRuntime::Unbox<CrossNetRuntime::BaseTypeWrapper<T>::BoxeableType >(value);
            Item((int)first, (int)second) = temp;
        }

        virtual void SetValue(System::Object * value, System::Int32 first, System::Int32 second, System::Int32 third)
        {
            T temp = CrossNetRuntime::Unbox<CrossNetRuntime::BaseTypeWrapper<T>::BoxeableType >(value);
            Item(first, second, third) = temp;
        }

        virtual void SetValue(System::Object * value, System::Int64 first, System::Int64 second, System::Int64 third)
        {
            T temp = CrossNetRuntime::Unbox<CrossNetRuntime::BaseTypeWrapper<T>::BoxeableType >(value);
            Item((int)first, (int)second, (int)third) = temp;
        }

        virtual System::Object * GetValue(System::Int32 first)
        {
            const T & temp = Item(first);
            return (CrossNetRuntime::Box<System::Object>(temp));
        }

        virtual System::Object * GetValue(System::Array__G<System::Int32> *)
        {
            CROSSNET_NOT_IMPLEMENTED();
            return (NULL);
        }

        virtual System::Object * GetValue(System::Int64 first)
        {
            const T & temp = Item((int)first);
            return (CrossNetRuntime::Box<System::Object>(temp));
        }

        virtual System::Object * GetValue(System::Array__G<System::Int64> *)
        {
            CROSSNET_NOT_IMPLEMENTED();
            return (NULL);
        }

        virtual System::Object * GetValue(System::Int32 first, System::Int32 second)
        {
            const T & temp = Item(first, second);
            return (CrossNetRuntime::Box<System::Object>(temp));
        }

        virtual System::Object * GetValue(System::Int64 first, System::Int64 second)
        {
            const T & temp = Item((int)first, (int)second);
            return (CrossNetRuntime::Box<System::Object>(temp));
        }

        virtual System::Object * GetValue(System::Int32 first, System::Int32 second, System::Int32 third)
        {
            const T & temp = Item(first, second, third);
            return (CrossNetRuntime::Box<System::Object>(temp));
        }

        virtual System::Object * GetValue(System::Int64 first, System::Int64 second, System::Int64 third)
        {
            const T & temp = Item((int)first, (int)second, (int)third);
            return (CrossNetRuntime::Box<System::Object>(temp));
        }

        int get_Length() const
        {
            return (GetSize());
        }

        // This method is an explicit implementation of ICollection.get_Count()
        // Keep it public for the wrappers...
        int ICollection__get_Count()
        {
            return (GetSize());
        }

        Int32 GetLength(Int32 dimension)
        {
            switch (dimension)
            {
            case 0:
                return (mFirst);

            case 1:
                CROSSNET_ASSERT(mSecond != 0, "This is not a two dimensions array!");
                return (mSecond);

            case 2:
                CROSSNET_ASSERT(mThird != 0, "This is not a three dimensions array!");
                return (mThird);

            default:
                CROSSNET_FAIL("Dimension is bigger than rank!");
                return (0);
            }
        }

        Int32 get_Rank()
        {
            if (mThird != 0)
            {
                return (3);
            }
            if (mSecond != 0)
            {
                return (2);
            }
            return (1);
        }

        Int32 GetLowerBound(Int32 dimension)
        {
            switch (dimension)
            {
            case 0:
                return (0);

            case 1:
                CROSSNET_ASSERT(mSecond != 0, "This is not a two dimensions array!");
                return (0);

            case 2:
                CROSSNET_ASSERT(mThird != 0, "This is not a three dimensions array!");
                return (0);

            default:
                CROSSNET_FAIL("Dimension is bigger than rank!");
                return (0);
            }
        }

        Int32 GetUpperBound(Int32 dimension)
        {
            switch (dimension)
            {
            case 0:
                return (mFirst - 1);

            case 1:
                CROSSNET_ASSERT(mSecond != 0, "This is not a two dimensions array!");
                return (mSecond - 1);

            case 2:
                CROSSNET_ASSERT(mThird != 0, "This is not a three dimensions array!");
                return (mThird - 1);

            default:
                CROSSNET_FAIL("Dimension is bigger than rank!");
                return (0);
            }
        }

        virtual void Reverse()
        {
            int size = GetSize();

            T * first = mItems;
            T * last = mItems + size - 1;

            T temp;

            while (first < last)
            {
                // Don't use copy constructor / operator, just copy byte by byte
                // The compiler should inline that correctly
                // Depending of the compiler / profiling we might want to actually use the copy operator
                __memcopy__(&temp, first, sizeof(T));
                __memcopy__(first, last, sizeof(T));
                __memcopy__(last, &temp, sizeof(T));
                ++first;
                --last;
            }
        }

        virtual void * * GetItemInterfaceMap()
        {
            return (CrossNetRuntime::GetInterfaceMap<T>());
        }

        class ArrayEnumerator : public System::Object
        {
        public:
            CN_MULTIPLE_DYNAMIC_OBJECT_ID
            (
                sizeof(ArrayEnumerator),
                __W3__
                (
                    CN_IMPLEMENT(ArrayEnumerator__IEnumerator),
                    CN_IMPLEMENT(ArrayEnumerator__IEnumerator__G1),
                    CN_IMPLEMENT(ArrayEnumerator__IDisposable)
                )
                ,
                NULL
            )

            ArrayEnumerator(Array__G * array)
                :
                mArray(array),
                mIndex(-1)
            {
                m__InterfaceMap__ = __GetInterfaceMap__();
                mSize = array->GetSize();
            }

            System::Object * get_Current()
            {
                return (CrossNetRuntime::Box<System::Object>(mArray->SingleDimensionItem(mIndex)));
            }

            T IEnumerator__G1__get_Current()
            {
                return (mArray->SingleDimensionItem(mIndex));
            }

            System::Boolean MoveNext()
            {
                ++mIndex;
                return (mIndex < mSize);
            }

            void Reset()
            {
                mIndex = -1;
            }

            void Dispose()
            {
                // Do nothing...
            }

            class ArrayEnumerator__IEnumerator : public System::Collections::IEnumerator
            {
            public:
                System::Object *    get_Current(void * __instance__)
                {
                    ArrayEnumerator * enumerator = static_cast<ArrayEnumerator *>(__instance__);
                    return (enumerator->get_Current());
                }

                System::Boolean     MoveNext(void * __instance__)
                {
                    ArrayEnumerator * enumerator = static_cast<ArrayEnumerator *>(__instance__);
                    return (enumerator->MoveNext());
                }

                void    Reset(void * __instance__)
                {
                    ArrayEnumerator * enumerator = static_cast<ArrayEnumerator *>(__instance__);
                    enumerator->Reset();
                }
            };

            class ArrayEnumerator__IEnumerator__G1 : public System::Collections::Generic::IEnumerator__G1<T>
            {
            public:
                T   get_Current(void * __instance__)
                {
                    ArrayEnumerator * enumerator = static_cast<ArrayEnumerator *>(__instance__);
                    return (enumerator->IEnumerator__G1__get_Current());
                }
            };

            class ArrayEnumerator__IDisposable : public System::IDisposable
            {
            public:
                void Dispose(void * __instance__)
                {
                    ArrayEnumerator * enumerator = static_cast<ArrayEnumerator *>(__instance__);
                    enumerator->Dispose();
                }
            };

        private:
            Array__G *  mArray;
            Int32       mIndex;
            Int32       mSize;
        };

        System::Collections::IEnumerator * GetEnumerator()
        {
            return (CrossNetRuntime::FastCast<System::Collections::IEnumerator>(new ArrayEnumerator(this)));
        }

        System::Collections::Generic::IEnumerator__G1<T> *  IEnumerator__G1__GetEnumerator()
        {
            return (CrossNetRuntime::FastCast<System::Collections::Generic::IEnumerator__G1<T> >(new ArrayEnumerator(this)));
        }

        System::Object * Clone()
        {
            Int32 rank = get_Rank();
            T * initValues = mItems;
            switch (rank)
            {
            case 1:
                return (__Create__(mFirst, initValues));

            case 2:
                return (__Create__(mFirst, mSecond, initValues));

            case 3:
                return (__Create__(mFirst, mSecond, mThird, initValues));

            default:
                CROSSNET_NOT_IMPLEMENTED();
                return (NULL);
            }
        }

        void IList__G1__Insert(::System::Int32 /*index*/, T /*item*/)
        {
            CROSSNET_DONT_CALL();
        }

        void IList__G1__Add(T /*item*/)
        {
            CROSSNET_DONT_CALL();
        }

        ::System::Boolean IList__G1__Remove(T /*item*/)
        {
            CROSSNET_DONT_CALL();
            return (false);
        }

        ::System::Int32 IndexOf(T item)
        {
            Int32 length = get_Length();

            // Optimized version should use traits to compare,
            // that way no boxing would be necessary nor slow Equals(Object *).
            System::Object * tempObj = CrossNetRuntime::Box<System::Object>(item);
            for (Int32 i = 0 ; i < length ; ++i)
            {
                if (CrossNetRuntime::GenWrapperConvert(mItems[i])->Equals(tempObj))
                {
                    return (i);
                }
            }
            return (-1);
        }

        ::System::Boolean Contains(T item)
        {
            Int32 length = get_Length();

            // Optimized version should use traits to compare,
            // that way no boxing would be necessary nor slow Equals(Object *).
            System::Object * tempObj = CrossNetRuntime::Box<System::Object>(item);
            for (Int32 i = 0 ; i < length ; ++i)
            {
                if (CrossNetRuntime::GenWrapperConvert(mItems[i])->Equals(tempObj))
                {
                    return (true);
                }
            }
            return (false);
        }

        virtual System::Int32 IndexOf(System::Object * value) const
        {
            Int32 length = get_Length();
            for (Int32 i = 0 ; i < length ; ++i)
            {
                if (CrossNetRuntime::GenWrapperConvert(mItems[i])->Equals(value))
                {
                    return (i);
                }
            }
            return (-1);
        }

        class Wrapper__ICloneable : public System::ICloneable
        {
        public:
            virtual System::Object *    Clone(void * __instance__)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->Clone());
            }
        };

        class Wrapper__IList : public System::Collections::IList
        {
        public:
            virtual System::Boolean get_IsFixedSize(void * __instance__)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->get_IsFixedSize());

            }
            virtual System::Boolean get_IsReadOnly(void * __instance__)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->get_IsReadOnly());
            }
            virtual System::Object * get_Item(void * __instance__, System::Int32 index)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->GetValue(index));
            }

            virtual System::Object * set_Item(void * __instance__, System::Int32 index, System::Object * value)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                __temp__->SetValue(value, index);
                return (value);
            }

            virtual System::Int32 Add(void * __instance__, System::Object * value)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->IList__Add(value));
            }

            virtual void Clear(void * __instance__)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                __temp__->IList__Clear();
            }

            virtual System::Boolean Contains(void * __instance__, System::Object * value)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->System::Array::Contains(value));          // Fully qualify it remove ambiguity
            }

            virtual System::Int32 IndexOf(void * __instance__, System::Object * value)
            {
                const Array__G * __temp__ = static_cast<const Array__G *>(__instance__);
                return (__temp__->Array__G::IndexOf(value));                // Fully qualify it remove ambiguity
            }

            virtual void Insert(void * __instance__, System::Int32 index, System::Object * value)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                __temp__->IList__Insert(index, value);
            }

            virtual void Remove(void * __instance__, System::Object * value)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                __temp__->IList__Remove(value);
            }

            virtual void RemoveAt(void * __instance__, System::Int32 index)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                __temp__->IList__RemoveAt(index);
            }
        };

        class Wrapper__ICollection : public System::Collections::ICollection
        {
        public:
            virtual System::Int32 get_Count(void * __instance__)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->ICollection__get_Count());
            }

            virtual System::Boolean get_IsSynchronized(void * __instance__)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->get_IsSynchronized());
            }
            virtual System::Object * get_SyncRoot(void * __instance__)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->get_SyncRoot());
            }
            virtual void CopyTo(void * __instance__, System::Array * array, System::Int32 index)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                __temp__->CopyTo(array, index);
            }
        };

        class Wrapper__IEnumerable : public System::Collections::IEnumerable
        {
        public:
            virtual System::Collections::IEnumerator * GetEnumerator(void * __instance__)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->GetEnumerator());
            }
        };

        class Wrapper__IList__G1 : public System::Collections::Generic::IList__G1<T>
        {
        public:
            virtual T get_Item(void * __instance__, ::System::Int32 index)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->Item(index));
            }
            virtual T set_Item(void * __instance__, ::System::Int32 index, T value)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->Item(index) = value);
            }
            virtual ::System::Int32 IndexOf(void * __instance__, T item)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->IndexOf(item));
            }
            virtual void Insert(void * __instance__, ::System::Int32 index, T item)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                __temp__->IList__G1__Insert(index, item);
            }
            virtual void RemoveAt(void * __instance__, ::System::Int32 index)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                __temp__->IList__RemoveAt(index);
            }
        };

        class Wrapper__ICollection__G1 : public System::Collections::Generic::ICollection__G1<T>
        {
        public:
            virtual ::System::Int32 get_Count(void * __instance__)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->get_Length());
            }
            virtual ::System::Boolean get_IsReadOnly(void * __instance__)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->get_IsReadOnly());
            }
            virtual void Add(void * __instance__, T item)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                __temp__->IList__G1__Add(item);
            }
            virtual void Clear(void * __instance__)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                __temp__->IList__Clear();
            }
            virtual ::System::Boolean Contains(void * __instance__, T item)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->Contains(item));
            }
            virtual void CopyTo(void * __instance__, typename ::System::Array__G< T > * array, ::System::Int32 arrayIndex)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                __temp__->CopyTo(array, arrayIndex);
            }
            virtual ::System::Boolean Remove(void * __instance__, T item)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->IList__G1__Remove(item));
            }
        };

        class Wrapper__IEnumerable__G1 : public System::Collections::Generic::IEnumerable__G1<T>
        {
        public:
            virtual System::Collections::Generic::IEnumerator__G1<T> *    GetEnumerator(void * __instance__)
            {
                Array__G * __temp__ = static_cast<Array__G *>(__instance__);
                return (__temp__->IEnumerator__G1__GetEnumerator());
            }
        };

        virtual void __Trace__(unsigned char currentMark)
        {
            ::CrossNetRuntime::Tracer::DoTrace(currentMark, mItems, GetSize());
        }

    protected:
        ~Array__G()
        {
            // Do nothing...
            // The items are not destroyed yet...
            // For classes it doesn't matter as pointer will be collected
            // But for struct with finalizers, they won't be called
            // We might want to create a traits to detect this case...
            // TODO: Add support for finalizers when T is a struct
        }

        virtual int GetSizeOfT()
        {
            return (sizeof(T));
        }

        virtual void * GetAddressOfFirstItem()
        {
            return (mItems);
        }

    private:
        explicit
        Array__G(int first, T * initValues = NULL)
            :   mFirst(first),
                mSecond(0),
                mThird(0)
        {
            Init(initValues);
        }

        Array__G(int first, int second, T * initValues = NULL)
            :   mFirst(first),
                mSecond(second),
                mThird(0)
        {
            Init(initValues);
        }

        Array__G(int first, int second, int third, T * initValues = NULL)
            :   mFirst(first),
                mSecond(second),
                mThird(third)
        {
            Init(initValues);
        }

        void Init(T * initValues)
        {
            m__InterfaceMap__ = __GetInterfaceMap__();

            int size = GetSize();
            // Copy the objects
            if (initValues != NULL)
            {
                memcpy(mItems, initValues, sizeof(T) * size);
            }
            else
            {
                // No initial values, set the memory to zero
                // Default value for base types (and thus struct members) as well as GC pointers...
                __memclear__(mItems, sizeof(T) * size);
            }
        }

        int GetSize() const
        {
            Int32   size = mFirst;
            if (mSecond != 0)
            {
                size *= mSecond;
                if (mThird != 0)
                {
                    size *= mThird;
                }
            }
            return (size);
        }

        // It seems that in fact, these three integers should actually be a dynamic C++ array
        Int32   mFirst;
        Int32   mSecond;
        Int32   mThird;
        mutable T mItems[0];
    };
}

#endif	//	__CROSSNET_ARRAY_H__

