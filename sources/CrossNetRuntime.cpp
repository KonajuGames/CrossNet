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

#include "CrossNetRuntime/CrossNetRuntime.h"
#include "CrossNetRuntime/System/IDisposable.h"
#include "CrossNetRuntime/System/Delegate.h"
#include "CrossNetRuntime/System/MulticastDelegate.h"
#include "CrossNetRuntime/System/CharEnumerator.h"
#include "CrossNetRuntime/System/Text/StringBuilder.h"
#include "CrossNetRuntime/Internal/Box.h"
#include "CrossNetRuntime/Internal/__Math__.h"
#include <math.h>

CrossNetRuntime::InitOptions CrossNetRuntime::InitOptions::sOptions;

System::String * CrossNetRuntime::BaseTypeWrapper<bool>::FalseString;
System::String * CrossNetRuntime::BaseTypeWrapper<bool>::TrueString;

const System::Single CrossNetRuntime::BaseTypeWrapper<System::Single>::Epsilon = FLT_EPSILON;
const System::Single CrossNetRuntime::BaseTypeWrapper<System::Single>::MaxValue = FLT_MAX;
const System::Single CrossNetRuntime::BaseTypeWrapper<System::Single>::MinValue = -FLT_MAX;
// Those values don't exist in limits.h...
const System::Single CrossNetRuntime::BaseTypeWrapper<System::Single>::NaN = FLT_MAX;                   // TODO:    Update this
const System::Single CrossNetRuntime::BaseTypeWrapper<System::Single>::NegativeInfinity = -FLT_MAX;     // TODO:    Update this
const System::Single CrossNetRuntime::BaseTypeWrapper<System::Single>::PositiveInfinity = +FLT_MAX;     // TODO:    Update this

const System::Double CrossNetRuntime::BaseTypeWrapper<System::Double>::Epsilon = DBL_EPSILON;
const System::Double CrossNetRuntime::BaseTypeWrapper<System::Double>::MaxValue = DBL_MAX;
const System::Double CrossNetRuntime::BaseTypeWrapper<System::Double>::MinValue = -DBL_MAX;
// Those values don't exist in limits.h...
const System::Double CrossNetRuntime::BaseTypeWrapper<System::Double>::NaN = DBL_MAX;                   // TODO:    Update this
const System::Double CrossNetRuntime::BaseTypeWrapper<System::Double>::NegativeInfinity = -DBL_MAX;     // TODO:    Update this
const System::Double CrossNetRuntime::BaseTypeWrapper<System::Double>::PositiveInfinity = +DBL_MAX;     // TODO:    Update this

const System::Decimal CrossNetRuntime::BaseTypeWrapper<System::Decimal>::MaxValue = +DBL_MAX;           // TODO:    Update this
const System::Decimal CrossNetRuntime::BaseTypeWrapper<System::Decimal>::MinValue = -DBL_MAX;           // TODO:    Update this
const System::Decimal CrossNetRuntime::BaseTypeWrapper<System::Decimal>::MinusOne = -1.0f;              // TODO:    Update this
const System::Decimal CrossNetRuntime::BaseTypeWrapper<System::Decimal>::One = +1.0f;                   // TODO:    Update this
const System::Decimal CrossNetRuntime::BaseTypeWrapper<System::Decimal>::Zero = +0.0f;                  // TODO:    Update this

void * * System::__Void__::s__InterfaceMap__ = NULL;

void * * CrossNetRuntime::BaseTypeWrapper<System::Boolean>::s__InterfaceMap__ = NULL;
void * * CrossNetRuntime::BaseTypeWrapper<System::SByte>::s__InterfaceMap__ = NULL;
void * * CrossNetRuntime::BaseTypeWrapper<System::Byte>::s__InterfaceMap__ = NULL;
void * * CrossNetRuntime::BaseTypeWrapper<System::Int16>::s__InterfaceMap__ = NULL;
void * * CrossNetRuntime::BaseTypeWrapper<System::UInt16>::s__InterfaceMap__ = NULL;
void * * CrossNetRuntime::BaseTypeWrapper<System::Char>::s__InterfaceMap__ = NULL;
void * * CrossNetRuntime::BaseTypeWrapper<System::Int32>::s__InterfaceMap__ = NULL;
void * * CrossNetRuntime::BaseTypeWrapper<System::UInt32>::s__InterfaceMap__ = NULL;
void * * CrossNetRuntime::BaseTypeWrapper<System::Int64>::s__InterfaceMap__ = NULL;
void * * CrossNetRuntime::BaseTypeWrapper<System::UInt64>::s__InterfaceMap__ = NULL;
void * * CrossNetRuntime::BaseTypeWrapper<System::Single>::s__InterfaceMap__ = NULL;
void * * CrossNetRuntime::BaseTypeWrapper<System::Double>::s__InterfaceMap__ = NULL;
void * * CrossNetRuntime::BaseTypeWrapper<System::Decimal>::s__InterfaceMap__ = NULL;

void CrossNetRuntime__PopulateInterfaceMaps()
{
    // Basic objects
    System::Object::__RegisterId__();
    System::__Void__::__RegisterId__();

    // Basic interfaces first so primitive objects can be registered
    System::IComparable::__RegisterId__();
    System::IFormattable::__RegisterId__();
    System::IDisposable::__RegisterId__();
    System::ICloneable::__RegisterId__();
    System::Collections::IEnumerator::__RegisterId__();
    System::Collections::IEnumerable::__RegisterId__();
    System::Collections::ICollection::__RegisterId__();
    System::Collections::IList::__RegisterId__();

    // Register string before CrossNetRuntime::BaseTypeWrapper<System::Boolean>
    //  As it registers False and True string but the interface map was not already set up correctly...
    System::String::__RegisterId__();

    CrossNetRuntime::BaseTypeWrapper<System::Boolean>::__RegisterId__();
    CrossNetRuntime::BaseTypeWrapper<System::SByte>::__RegisterId__();
    CrossNetRuntime::BaseTypeWrapper<System::Byte>::__RegisterId__();
    CrossNetRuntime::BaseTypeWrapper<System::Int16>::__RegisterId__();
    CrossNetRuntime::BaseTypeWrapper<System::UInt16>::__RegisterId__();
    CrossNetRuntime::BaseTypeWrapper<System::Char>::__RegisterId__();
    CrossNetRuntime::BaseTypeWrapper<System::Int32>::__RegisterId__();
    CrossNetRuntime::BaseTypeWrapper<System::UInt32>::__RegisterId__();
    CrossNetRuntime::BaseTypeWrapper<System::Int64>::__RegisterId__();
    CrossNetRuntime::BaseTypeWrapper<System::UInt64>::__RegisterId__();
    CrossNetRuntime::BaseTypeWrapper<System::Single>::__RegisterId__();
    CrossNetRuntime::BaseTypeWrapper<System::Double>::__RegisterId__();
    CrossNetRuntime::BaseTypeWrapper<System::Decimal>::__RegisterId__();

    // Then more complex base types
    System::Delegate::__RegisterId__();
    System::MulticastDelegate::__RegisterId__();

    // Finally the more complex types
    System::CharEnumerator::__RegisterId__();
    System::Text::StringBuilder::__RegisterId__();
}

void CrossNetRuntime::BaseTypeWrapper<System::Boolean>::__RegisterId__()
{
    CrossNetRuntime::InterfaceInfo info[] = 
    {
        CN_IMPLEMENT(Wrapper__IComparable),
        //CN_IMPLEMENT(Wrapper__IConvertible),
        CN_IMPLEMENT(Wrapper__IComparable__G1),
        CN_IMPLEMENT(Wrapper__IEquatable__G1),
    };
    s__InterfaceMap__ = CrossNetRuntime::InterfaceMapper::RegisterObject(sizeof(CrossNetRuntime::BoxedObject<CrossNetRuntime::BaseTypeWrapper<System::Boolean> >), info, sizeof(info) / sizeof(info[0]), NULL);

    FalseString = ::CrossNetRuntime::StringPooler::GetOrCreateString(L"False");
    TrueString = ::CrossNetRuntime::StringPooler::GetOrCreateString(L"True");
}

#define IMPLEMENT_REGISTER_ID(type)                                                 \
void CrossNetRuntime::BaseTypeWrapper<type>::__RegisterId__()                   \
{                                                                                   \
    CrossNetRuntime::InterfaceInfo info[] =                                         \
    {                                                                               \
        CN_IMPLEMENT(Wrapper__IComparable),                                         \
        CN_IMPLEMENT(Wrapper__IFormattable),                                        \
        /*CN_IMPLEMENT(Wrapper__IConvertible),*/                                    \
        CN_IMPLEMENT(Wrapper__IComparable__G1),                                     \
        CN_IMPLEMENT(Wrapper__IEquatable__G1),                                      \
    };                                                                              \
    s__InterfaceMap__ = CrossNetRuntime::InterfaceMapper::RegisterObject(sizeof(CrossNetRuntime::BoxedObject<CrossNetRuntime::BaseTypeWrapper<type> >), info, sizeof(info) / sizeof(info[0]), NULL);   \
}

IMPLEMENT_REGISTER_ID(::System::Byte)
IMPLEMENT_REGISTER_ID(::System::SByte)
IMPLEMENT_REGISTER_ID(::System::Char)
IMPLEMENT_REGISTER_ID(::System::Int16)
IMPLEMENT_REGISTER_ID(::System::UInt16)
IMPLEMENT_REGISTER_ID(::System::Int32)
IMPLEMENT_REGISTER_ID(::System::UInt32)
IMPLEMENT_REGISTER_ID(::System::Int64)
IMPLEMENT_REGISTER_ID(::System::UInt64)
IMPLEMENT_REGISTER_ID(::System::Single)
IMPLEMENT_REGISTER_ID(::System::Double)
IMPLEMENT_REGISTER_ID(::System::Decimal)

//template <>       //  The class is already specialized, so we just need to define the method (don't define it as specialized)
                    //  Also because that's a specialization, if it is not inlined, this should not be defined in a header...
System::String * CrossNetRuntime::BaseTypeWrapper<System::Int32>::ToString(System::String * format)
{
    // Partially supported... Currently only so the unit-test can run...
    const System::Char * buffer = format->__ToCString__();
    if (format->get_Length() == 1)
    {
        System::Char  text[16];
        if (buffer[0] == L'X')
        {
            _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%X", mValue);
            return (System::String::__Create__(text));
        }
        else if (buffer[0] == L'x')
        {
            _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%x", mValue);
            return (System::String::__Create__(text));
        }
    }
    return (ToString());
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::Char>::ToString(System::String * /*format*/, System::IFormatProvider * /*formatProvider*/)
{
    CROSSNET_NOT_IMPLEMENTED();
    return (NULL);
}

System::Double CrossNetRuntime::__Math__::Modulo(System::Double a, System::Double b)
{
    // Something like that...
    double  f = a / b;
    f = floor(f) * b;
    return (a - f);
}

System::Int32 CrossNetRuntime::BaseTypeWrapper<System::Int32>::Parse(System::String * value)
{
    int integer = _wtoi(value->__ToCString__());
    // Currently this code doesn't handle exceptions correctly if the conversion is incorrect...
    // TODO:    Manage exceptions accordingly...
    return (integer);
}

System::Int16 CrossNetRuntime::BaseTypeWrapper<System::Int16>::Parse(System::String * value)
{
    int integer = _wtoi(value->__ToCString__());
    // Currently this code doesn't handle exceptions correctly if the conversion is incorrect...
    // TODO:    Manage exceptions accordingly...
    return (System::Int16)(integer);
}

System::UInt16 CrossNetRuntime::BaseTypeWrapper<System::UInt16>::Parse(System::String * value)
{
    int integer = _wtoi(value->__ToCString__());
    // Currently this code doesn't handle exceptions correctly if the conversion is incorrect...
    // TODO:    Manage exceptions accordingly...
    return (System::UInt16)(integer);
}

System::Decimal CrossNetRuntime::BaseTypeWrapper<System::Decimal>::Parse(System::String * value, System::IFormatProvider * /* formatProvider */)
{
    double d = _wtof(value->__ToCString__());
    // Currently this code doesn't handle exceptions correctly if the conversion is incorrect...
    // TODO:    Manage exceptions accordingly...
    return (d);
}

// If you change this code, make sure that the parser is updated accoordingly...
// And the other way around too
//  Search for CppUtil.GetStringHashCode in CrossNet assembly
System::Int32 CrossNetRuntime::GetHashCode(void * bufferToCrc, System::Int32 length)
{
    // The assumption is that the hashcode is calculated during parsing with FNV1
    // We certainly want to implement a simpler / faster algorithm with some other optimizations... like loop unrolling, etc...
    const System::UInt32    OFFSET_BASIS = 2166136261;
    const System::UInt32    FNV_PRIME = 16777619;

    System::UInt32 hashCode = OFFSET_BASIS;
    char * buffer = (char *)bufferToCrc;
    char * endBuffer = buffer + length;
    while (buffer < endBuffer)
    {
        hashCode ^= (System::UInt32)(*buffer++);
        System::UInt64 result = (System::UInt64)hashCode * (System::UInt64)FNV_PRIME;
        hashCode = (System::UInt32)result;
    }
    return (System::Int32)(hashCode);
}

System::Int32 CrossNetRuntime::GetHashCodeForString(::System::Char * textToCrc, System::Int32 length)
{
    // The assumption is that the hashcode is calculated during parsing with FNV1
    // We certainly want to implement a simpler / faster algorithm with some other optimizations... like loop unrolling, etc...
    const System::UInt32    OFFSET_BASIS = 2166136261;
    const System::UInt32    FNV_PRIME = 16777619;

    System::UInt32 hashCode = OFFSET_BASIS;
    System::Char * buffer = textToCrc;
    System::Char * endBuffer = buffer + length;
    while (buffer < endBuffer)
    {
        hashCode ^= (System::UInt32)(*buffer++);
        System::UInt64 result = (System::UInt64)hashCode * (System::UInt64)FNV_PRIME;
        hashCode = (System::UInt32)result;
    }
    return (System::Int32)(hashCode);
}

void CrossNetRuntime::Setup(const CrossNetRuntime::InitOptions & options)
{
    CrossNetRuntime::InitOptions::sOptions = options;

    CrossNetRuntime::GCAllocator::Setup(options);
    CrossNetRuntime::GCManager::Setup(options);
    CrossNetRuntime::InterfaceMapper::Setup(options);

    CrossNetRuntime__PopulateInterfaceMaps();
}

void CrossNetRuntime::Teardown()
{
    CrossNetRuntime::InterfaceMapper::Teardown();
    CrossNetRuntime::GCManager::Teardown();
    CrossNetRuntime::GCAllocator::Teardown();
}

const CrossNetRuntime::InitOptions & CrossNetRuntime::GetOptions()
{
    return (CrossNetRuntime::InitOptions::sOptions);
}

void CrossNetRuntime::Trace(unsigned char currentMark)
{
    // Trace the types...
    InterfaceMapper::Trace(currentMark);

    // Trace pooled strings
    StringPooler::Trace(currentMark);

    // Trace static members
    // This is actually already traced by the string pooler
    //GCManager::Trace(System::String::Empty, currentMark);
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::Boolean>::ToString()
{
    if (mValue == true)
    {
        return (TrueString);
    }
    else
    {
        return (FalseString);
    }
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::Byte>::ToString()
{
    System::Char  text[16];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%c", mValue);
    return (System::String::__Create__(text));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::SByte>::ToString()
{
    System::Char  text[16];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%c", mValue);
    return (System::String::__Create__(text));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::Char>::ToString()
{
    // Don't use _snwprintf_s, as in some cases (like \xffff), the buffer won't be incorrectly filled...
    System::Char  text[2];
    text[0] = mValue;
    text[1] = '\0';
    // TODO: We might want to use the stringpooler instead
    return (System::String::__CreateWithLengthKnown__(text, 1));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::Int16>::ToString()
{
    System::Char  text[16];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%d", mValue);
    return (System::String::__Create__(text));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::Int16>::ToString(System::String * /* format */)
{
    System::Char  text[16];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%d", mValue);
    return (System::String::__Create__(text));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::UInt16>::ToString()
{
    System::Char  text[16];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%u", mValue);
    return (System::String::__Create__(text));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::UInt16>::ToString(System::String * /* format */)
{
    System::Char  text[16];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%d", mValue);
    return (System::String::__Create__(text));
}

template <>
System::String * CrossNetRuntime::CommonBaseTypeWrapper<System::Int32>::ToString()
{
    System::Char  text[16];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%d", mValue);
    return (System::String::__Create__(text));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::UInt32>::ToString()
{
    System::Char  text[16];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%d", mValue);
    return (System::String::__Create__(text));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::UInt32>::ToString(System::String * /* format */)
{
    System::Char  text[16];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%d", mValue);
    return (System::String::__Create__(text));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::Int64>::ToString()
{
    System::Char  text[32];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%ld", mValue);
    return (System::String::__Create__(text));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::UInt64>::ToString()
{
    System::Char  text[32];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%lu", mValue);
    return (System::String::__Create__(text));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::Int64>::ToString(System::String * /*format*/)
{
    System::Char  text[32];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%ld", mValue);
    return (System::String::__Create__(text));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::UInt64>::ToString(System::String * /*format*/)
{
    System::Char  text[32];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%lu", mValue);
    return (System::String::__Create__(text));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::Single>::ToString()
{
    System::Char  text[16];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%f", mValue);
    return (System::String::__Create__(text));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::Double>::ToString()
{
    System::Char  text[16];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%f", mValue);
    return (System::String::__Create__(text));
}

System::String * CrossNetRuntime::BaseTypeWrapper<System::Decimal>::ToString()
{
    System::Char  text[16];
    _snwprintf_s(text, CN_ARRAY_SIZE(text), CN_ARRAY_SIZE(text), L"%f", mValue);
    return (System::String::__Create__(text));
}