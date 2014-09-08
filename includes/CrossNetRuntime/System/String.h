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

#ifndef __SYSTEM_STRING_H__
#define __SYSTEM_STRING_H__

#include "CrossNetRuntime/System/Object.h"
#include "CrossNetRuntime/System/IComparable.h"
#include "CrossNetRuntime/System/ICloneable.h"
#include "CrossNetRuntime/System/IComparable__G1.h"
#include "CrossNetRuntime/System/Collections/Generic/IEnumerable__G1.h"
#include "CrossNetRuntime/System/Collections/IEnumerable.h"
#include "CrossnetRuntime/System/IEquatable__G1.h"

namespace CrossNetRuntime
{
    template <typename T>
    class PointerWrapper;

    class StringPooler;

    template <typename T>
    class BaseTypeWrapper;
}

namespace System
{
    class CharEnumerator;
    struct StringComparison;
    class IFormatProvider;

    namespace Globalization
    {
        class CultureInfo;
    }

    namespace Text
    {
        class StringBuilder;
        struct NormalizationForm;
    }

    template <typename T>
    class Array__G;

    class String : public System::Object
	{
	public:
        CN_DYNAMIC_ID()

        static String * __Create__(const System::Char * text);

        static String * __Create__(System::Char * text, System::Int32 start, System::Int32 length);
//        static String * __Create__(CrossNetRuntime::PointerWrapper<System::Char *> text, System::Int32 start, System::Int32 length);
        static String * __Create__(System::Array__G<System::Char> * text, System::Int32 start, System::Int32 length);
        static String * __Create__(System::Array__G<System::Char> * text);

        static String * __Create__(const System::Char c, int number);

        // Create a string from a given size... (trailing '\0' counted as well)
        static String * __Create__(System::Int32 totalSize);

        static String * __Create__(CrossNetRuntime::PointerWrapper<System::SByte *>);
        static String * __Create__(CrossNetRuntime::PointerWrapper<System::SByte *>, System::Int32, System::Int32);

        static System::Boolean IsInterned(System::String *);
        static System::String * Copy(System::String *);
        static System::String * Intern(System::String *);

        System::Array__G<String *> *    Split(String * s);
        System::Array__G<String *> *    Split(System::Array__G<Char> * array);
        System::Array__G<String *> *    Split(System::Array__G<Char> * array, System::Int32);
        static System::String *         Join(System::String *, System::Array__G<String *> *);
        static System::String *         Join(System::String *, System::Array__G<String *> *, System::Int32, System::Int32);
        System::String *                Insert(System::Int32 startIndex, System::String * value);
        System::Boolean                 Contains(String * value);

        static String * Concat(System::Object *);
        static String * Concat(System::Array__G<System::Object *> * array);
        static String * Concat(System::Array__G<System::String *> * array);

		static String * Concat(const String * s1, const String * s2);
		static String * Concat(const String * s1, const String * s2, const String * s3);
		static String * Concat(const String * s1, const String * s2, const String * s3, const String * s4);
        static String * Concat(System::Object * a, System::Object * b);
        static String * Concat(System::Object * a, System::Object * b, System::Object * c);
        static String * Concat(System::Object * a, System::Object * b, System::Object * c, System::Object * d);

        static String * Format(String * /*a*/, Object * /*b*/);
        static String * Format(String * /*a*/, Object * /*b*/, Object * /*c*/);
        static String * Format(String * /*a*/, Object * /*b*/, Object * /*c*/, Object * /*d*/);
        static String * Format(String * /*a*/, System::Array__G<Object *> * /*array*/);
        static String * Format(IFormatProvider * formatProvider, String * /*a*/, System::Array__G<Object *> * /*array*/);

        CROSSNET_FINLINE
        System::Int32 get_Length() const
        {
            return (mLength);
        }

        System::Int32 set_Length(System::Int32);
        void    SetLength(System::Int32);

        CROSSNET_FINLINE
        System::Char get_Item(System::Int32 index) const
        {
            CROSSNET_ASSERT(index >= 0, "");
            CROSSNET_ASSERT(index < mLength, "");
            return (mBuffer[index]);
        }

        CROSSNET_FINLINE
        virtual System::String * ToString()
        {
            // The simplest implementation...
            return (this);
        }

        virtual System::Int32 GetHashCode();
        virtual int __GetVariableSize__();

        static int Compare(const String * a, const String * b);

        System::Int32 CompareTo(System::Object * value);

        System::Int32 CompareTo(System::String * value)
        {
            return (Compare(this, value));
        }

        static bool op_Equality(const String * a, const String * b)
        {
            if (a->mLength != b->mLength)
            {
                return (false);
            }
            return (Compare(a, b) == 0);
        }

        virtual System::Boolean Equals(System::Object * obj);

        System::Boolean Equals(System::String * text, StringComparison);
        static System::Boolean Equals(System::String * a, System::String * b)
        {
            return (op_Equality(a, b));
        }

        static System::Boolean op_Inequality(const String * a, const String * b)
        {
            if (a->mLength != b->mLength)
            {
                return (true);
            }
            return (Compare(a, b) != 0);
        }

        static System::Int32 Compare(const String * a, const String * b, System::Boolean ignoreCase);
        static System::Int32 Compare(const String * a, const String * b, StringComparison comparisonType);
        static System::Int32 Compare(const String * a, const String * b, System::Boolean, System::Globalization::CultureInfo *);
        static System::Int32 Compare(const String * a, System::Int32, const String * b, System::Int32, System::Int32);
        static System::Int32 Compare(const String * a, System::Int32, const String * b, System::Int32, System::Int32, System::Boolean);
        static System::Int32 Compare(const String * a, System::Int32, const String * b, System::Int32, System::Int32, StringComparison comparisonType);
        static System::Int32 Compare(const String * a, System::Int32, const String * b, System::Int32, System::Int32, System::Boolean, System::Globalization::CultureInfo *);

        String * Trim();
        String * Trim(System::Array__G<System::Char> *);
        System::String * TrimStart(System::Array__G<System::Char> *);
        System::String * TrimEnd(System::Array__G<System::Char> *);

        CharEnumerator * GetEnumerator();

        CROSSNET_FINLINE
        const System::Char * __ToCString__() const
        {
            return (mBuffer);
        }

        CROSSNET_FINLINE
        System::Char * __ToCString__()
        {
            return (mBuffer);
        }

        CROSSNET_FINLINE
        static
        System::Boolean IsNullOrEmpty(System::String * text)
        {
            return (text == NULL ? true : (text->mLength == 0));
        }

        static System::String * GetStringForStringBuilder(System::String *, System::Int32);
        static System::String * GetStringForStringBuilder(System::String *, System::Int32, System::Int32, System::Int32);

        System::Boolean StartsWith(System::Char value);
        System::Boolean StartsWith(System::String * text);
        System::Boolean StartsWith(System::String * text, StringComparison);
        System::Boolean EndsWith(System::Char value);
        System::Boolean EndsWith(System::String * text);
        System::Boolean EndsWith(System::String * text, StringComparison);

        System::String * Substring(int start);
        System::String * Substring(int start, int length);

        System::Int32 IndexOf(System::Char);
        System::Int32 IndexOf(System::String *);
        System::Int32 IndexOf(System::Char, System::Int32);
        System::Int32 IndexOf(System::String *, System::Int32);
        System::Int32 IndexOf(System::String *, StringComparison);
        System::Int32 IndexOf(System::Char, System::Int32, System::Int32);
        System::Int32 IndexOf(System::String *, System::Int32, System::Int32);
        System::Int32 IndexOf(System::String *, System::Int32, StringComparison);
        System::Int32 IndexOf(System::String *, System::Int32, System::Int32, StringComparison);
        System::Int32 IndexOfAny(System::Array__G<System::Char> *);
        System::Int32 IndexOfAny(System::Array__G<System::Char> *, System::Int32);
        System::Int32 IndexOfAny(System::Array__G<System::Char> *, System::Int32, System::Int32);

        System::Int32 LastIndexOf(System::Char);
        System::Int32 LastIndexOf(System::String *);
        System::Int32 LastIndexOf(System::Char, System::Int32);
        System::Int32 LastIndexOf(System::String *, System::Int32);
        System::Int32 LastIndexOf(System::String *, StringComparison);
        System::Int32 LastIndexOf(System::Char, System::Int32, System::Int32);
        System::Int32 LastIndexOf(System::String *, System::Int32, System::Int32);
        System::Int32 LastIndexOf(System::String *, System::Int32, StringComparison);
        System::Int32 LastIndexOf(System::String *, System::Int32, System::Int32, StringComparison);
        System::Int32 LastIndexOfAny(System::Array__G<System::Char> *);
        System::Int32 LastIndexOfAny(System::Array__G<System::Char> *, System::Int32);
        System::Int32 LastIndexOfAny(System::Array__G<System::Char> *, System::Int32, System::Int32);

        static System::Int32 CompareOrdinal(System::String *, System::String *);
        static System::Int32 CompareOrdinal(System::String *, System::Int32, System::String *, System::Int32, System::Int32);
        System::String * Remove(System::Int32);
        System::String * Remove(System::Int32, System::Int32);
        System::String * Replace(System::Char, System::Char);
        System::String * Replace(System::String *, System::String *);
        System::String * ToLower();
        System::String * ToLower(System::Globalization::CultureInfo *);
        System::String * ToUpper();
        System::String * ToUpper(System::Globalization::CultureInfo *);
        System::Array__G<System::Char> * ToCharArray();
        System::Array__G<System::Char> * ToCharArray(System::Int32, System::Int32);
        void CopyTo(System::Int32 sourceIndex, System::Array__G<System::Char> * destination, System::Int32 destinationIndex, System::Int32 count);
        System::String * Normalize();
        System::String * Normalize(System::Text::NormalizationForm);

        System::String * PadRight(::System::Int32);
        System::String * PadRight(::System::Int32, ::System::Char);
        System::String * PadLeft(::System::Int32);
        System::String * PadLeft(::System::Int32, ::System::Char);

        static System::Int32 nativeCompareOrdinal(String *, System::String *, System::Boolean);
        static System::Int32 nativeCompareOrdinalEx(String *, System::Int32, System::String *, System::Int32, System::Int32);
        static System::Int32 nativeCompareOrdinalWC(String *, System::Char *, System::Boolean, System::Boolean *);

        static String * Empty;

    private:

        String(const System::Char * text, System::Int32 length);
        explicit String(System::Int32 size);
        String(System::Char c, int number);
#if 0
        String(const System::Char * text, System::Int32 size, bool safe);
#endif
        String(const System::Char * text, System::Int32 startIndex, System::Int32 size);

        // Destructor can be private, this class is sealed...
        virtual ~String();

        static String * __CreateEmpty__();

        // Specific version where the size of the string is known
        static String * __CreateWithLengthKnown__(System::Char * text, System::Int32 length);

        class Wrapper__IComparable : public IComparable
        {
        public:
            virtual System::Int32 CompareTo(void * __instance__, System::Object * other)
            {
                String * instance = static_cast<String *>(__instance__);
                return (instance->CompareTo(other));
            }
        };

        class Wrapper__ICloneable : public ICloneable
        {
        public:
            virtual System::Object *    Clone(void * __instance__)
            {
                String * instance = static_cast<String *>(__instance__);
                // Strings are immutable, no real need to clone, we return the same object
                // In reality, there might be some cases where it would be better to really clone...
                return (instance);
            }

        };

        class Wrapper__IComparable__G1 : public IComparable__G1<String *>
        {
        public:
            virtual System::Int32 CompareTo(void * __instance__, System::String * other)
            {
                String * instance = static_cast<String *>(__instance__);
                return (instance->CompareTo(other));
            }
        };

        class Wrapper__IEnumerable__G1 : public ::System::Collections::Generic::IEnumerable__G1<String *>
        {
        public:
            virtual ::System::Collections::Generic::IEnumerator__G1<String *> *    GetEnumerator(void * __instance__)
            {
                __instance__;
                CROSSNET_NOT_IMPLEMENTED();
                return (NULL);
            }
        };

        class Wrapper__IEnumerable : public ::System::Collections::IEnumerable
        {
        public:
            virtual System::Collections::IEnumerator * GetEnumerator(void * __instance__)
            {
                __instance__;
                CROSSNET_NOT_IMPLEMENTED();
                return (NULL);
            }
        };

        class Wrapper__IEquatable__G1 : public IEquatable__G1<String *>
        {
        public:
            virtual System::Boolean Equals(void * __instance__, String * other)
            {
                String * instance = static_cast<String *>(__instance__);
                return (instance->Equals(other));
            }
        };

        System::Int32   mHashCode;
        System::Int32   mLength;

#pragma warning(disable: 4200)
        System::Char    mBuffer[0];

        friend class System::Text::StringBuilder;
        friend class CrossNetRuntime::StringPooler;

        // There is also one case where this is necessary (creating a string from a char than can be zero)
        // TODO: Revisit this later...
        friend class CrossNetRuntime::BaseTypeWrapper<System::Char>;
	};
}

#endif