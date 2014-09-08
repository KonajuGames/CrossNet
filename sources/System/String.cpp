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

#include "CrossNetRuntime/System/String.h"

#include "CrossNetRuntime/CrossNetRuntime.h"
#include "CrossNetRuntime/Internal/BaseTypes.h"
#include "CrossNetRuntime/System/CharEnumerator.h"
#include "CrossNetRuntime/System/StringComparison.h"
#include "CrossNetRuntime/System/Text/StringBuilder.h"

namespace System
{

String * String::Empty = NULL;
void * * String::s__InterfaceMap__ = NULL;

void String::__RegisterId__()
{
    CrossNetRuntime::InterfaceInfo info[] =
    {
        CN_IMPLEMENT(Wrapper__IComparable),
        CN_IMPLEMENT(Wrapper__ICloneable),
        /*CN_IMPLEMENT(Wrapper__IConvertible),*/
        CN_IMPLEMENT(Wrapper__IComparable__G1),
        CN_IMPLEMENT(Wrapper__IEnumerable__G1),
        CN_IMPLEMENT(Wrapper__IEnumerable),
        CN_IMPLEMENT(Wrapper__IEquatable__G1),
    };
    s__InterfaceMap__ = CrossNetRuntime::InterfaceMapper::RegisterObject(sizeof(System::String), info, sizeof(info) / sizeof(info[0]), NULL);

    // Call a specific function so we are not using the empty string (that we are trying to create)...
    Empty = String::__CreateEmpty__();
    CrossNetRuntime::StringPooler::AddString(Empty);
}

String::String(const System::Char * text, System::Int32 length)
    :
    Object(__STRING__),
    mHashCode(0)        // By default, hash code is 0, it means it is not calculated yet
{
    m__InterfaceMap__ = __GetInterfaceMap__();
    CROSSNET_ASSERT(m__InterfaceMap__ != NULL, "Incorrect init order for string!");
    mLength = length;
    wmemcpy(mBuffer, text, mLength + 1);
}

String::String(System::Char c, int number)
    :
    Object(__STRING__),
    mHashCode(0)        // By default, hash code is 0, it means it is not calculated yet
{
    m__InterfaceMap__ = __GetInterfaceMap__();
    CROSSNET_ASSERT(m__InterfaceMap__ != NULL, "Incorrect init order for string!");
    mLength = number;
    wmemset(mBuffer, c, mLength);
    mBuffer[mLength] = L'\0';
}

String::String(System::Int32 size)
    :
    Object(__STRING__),
    mHashCode(0)        // By default hash code is 0, it means it is not calculated yet
{
    CROSSNET_ASSERT(size > 0, "The size passed is incorrect!");
    m__InterfaceMap__ = __GetInterfaceMap__();
    CROSSNET_ASSERT(m__InterfaceMap__ != NULL, "Incorrect init order for string!");
    mLength = size - 1;
}

#if 0
String::String(const System::Char * text, System::Int32 size, bool safe)
    :
    Object(__STRING__),
    mHashCode(0)        // By default hash code is 0, it means it is not calculated yet
{
    m__InterfaceMap__ = __GetInterfaceMap__();
    CROSSNET_ASSERT(m__InterfaceMap__ != NULL, "Incorrect init order for string!");
    if (safe)
    {
        mLength = size;
        CROSSNET_ASSERT((System::Int32)wcslen(text) == size, "");
    }
    else
    {
        Int32 length = (System::Int32)wcslen(text);
        mLength = length;
        if (mLength > size)
        {
            mLength = size;
        }
    }
    wmemcpy(mBuffer, text, mLength + 1);
}
#endif

String::String(const System::Char * text, System::Int32 startIndex, System::Int32 size)
    :
    Object(__STRING__),
    mHashCode(0)        // By default hash code is 0, it means it is not calculated yet
{
    m__InterfaceMap__ = __GetInterfaceMap__();
    CROSSNET_ASSERT(m__InterfaceMap__ != NULL, "Incorrect init order for string!");
    mLength = size;
    wmemcpy(mBuffer, text + startIndex, size);
    mBuffer[mLength] = L'\0';
}

// Destructor can be private, this class is sealed...
String::~String()
{
    // Do nothing...
}

String * String::__CreateEmpty__()
{
    String * empty = __Create__(1);
    empty->mBuffer[0] = L'\0';
    return (empty);
}

String * String::__Create__(const System::Char * text)
{
    if (text[0] == '\0')
    {
        return (Empty);
    }
    int length = (System::Int32)wcslen(text);
    String * temp = (String *)operator new(sizeof(String) + ((length + 1) * sizeof(System::Char)));
    temp->String::String(text, length);
    return (temp);
}

String * String::__Create__(System::Char * text, System::Int32 start, System::Int32 length)
{
    if (length <= 0)
    {
        return (Empty);
    }
    Int32 totalLength = (System::Int32)wcslen(text);
    CROSSNET_ASSERT(totalLength >= start, "The startIndex is smaller than the length of the string!");
    int localLength = totalLength - start;
    if (localLength > length)
    {
        localLength = length;
    }
    String * temp = (String *)operator new (sizeof(String) + ((localLength + 1) * sizeof(System::Char)));
    temp->String::String(text, start, localLength);
    return (temp);
}

String * String::__CreateWithLengthKnown__(System::Char * text, System::Int32 length)
{
    if (length <= 0)
    {
        return (Empty);
    }
    String * temp = (String *)operator new (sizeof(String) + ((length + 1) * sizeof(System::Char)));
    temp->String::String(text, 0, length);
    return (temp);
}

System::CharEnumerator * String::GetEnumerator()
{
    return (System::CharEnumerator::__Create__(this));
}

String * String::__Create__(const System::Char c, int number)
{
    if (number <= 0)
    {
        return (Empty);
    }
    String * temp = (String *)operator new (sizeof(String) + ((number + 1) * sizeof(System::Char)));
    temp->String::String(c, number);
    return (temp);
}

String * String::__Create__(System::Int32 totalSize)
{
    if (totalSize <= 0)
    {
        return (Empty);
    }
    String * temp = (String *)operator new (sizeof(String) + (totalSize * sizeof(System::Char)));
    temp->String::String(totalSize);
    return (temp);
}

String * String::Concat(const String * s1, const String * s2)
{
    System::Int32 size1 = s1->get_Length();
    System::Int32 size2 = s2->get_Length();
    System::Int32 bufferSize = size1 + size2 + 1;   // +1 for the trailing '\0'
    System::String * concatString = __Create__(bufferSize);
    System::Char * buffer = const_cast<System::Char *>(concatString->__ToCString__());
    wmemcpy(buffer, s1->__ToCString__(), size1);
    wmemcpy(buffer + size1, s2->__ToCString__(), size2 + 1);
    return (concatString);
}

String * String::Concat(const String * s1, const String * s2, const String * s3)
{
    System::Int32 size1 = s1->get_Length();
    System::Int32 size2 = s2->get_Length();
    System::Int32 size3 = s3->get_Length();
    System::Int32 bufferSize = size1 + size2 + size3 + 1;   // +1 for the trailing '\0'
    System::String * concatString = __Create__(bufferSize);
    System::Char * buffer = const_cast<System::Char *>(concatString->__ToCString__());
    wmemcpy(buffer, s1->__ToCString__(), size1);
    wmemcpy(buffer + size1, s2->__ToCString__(), size2);
    wmemcpy(buffer + (size1 + size2), s3->__ToCString__(), size3 + 1);
    return (concatString);
}

String * String::Concat(const String * s1, const String * s2, const String * s3, const String * s4)
{
    System::Int32 size1 = s1->get_Length();
    System::Int32 size2 = s2->get_Length();
    System::Int32 size3 = s3->get_Length();
    System::Int32 size4 = s4->get_Length();
    System::Int32 bufferSize = size1 + size2 + size3 + size4 + 1;   // +1 for the trailing '\0'
    System::String * concatString = __Create__(bufferSize);
    System::Char * buffer = const_cast<System::Char *>(concatString->__ToCString__());
    wmemcpy(buffer, s1->__ToCString__(), size1);
    wmemcpy(buffer + size1, s2->__ToCString__(), size2);
    wmemcpy(buffer + (size1 + size2), s3->__ToCString__(), size3);
    wmemcpy(buffer + (size1 + size2 + size3), s4->__ToCString__(), size4 + 1);
    return (concatString);
}

String * String::Concat(System::Object * a, System::Object * b)
{
    System::String * s1 = a->ToString();
    System::String * s2 = b->ToString();
    return (Concat(s1, s2));
}

String * String::Concat(System::Object * a, System::Object * b, System::Object * c)
{
    System::String * s1 = a->ToString();
    System::String * s2 = b->ToString();
    System::String * s3 = c->ToString();

    System::Int32 size1 = s1->get_Length();
    System::Int32 size2 = s2->get_Length();
    System::Int32 size3 = s3->get_Length();
    System::Int32 bufferSize = size1 + size2 + size3 + 1;   // +1 for the trailing '\0'
    System::String * concatString = __Create__(bufferSize);
    System::Char * buffer = const_cast<System::Char *>(concatString->__ToCString__());
    wmemcpy(buffer, s1->__ToCString__(), size1);
    wmemcpy(buffer + size1, s2->__ToCString__(), size2);
    wmemcpy(buffer + (size1 + size2), s3->__ToCString__(), size3 + 1);
    return (concatString);
}

String * String::Concat(System::Object * a, System::Object * b, System::Object * c, System::Object * d)
{
    System::String * s1 = a->ToString();
    System::String * s2 = b->ToString();
    System::String * s3 = c->ToString();
    System::String * s4 = d->ToString();

    System::Int32 size1 = s1->get_Length();
    System::Int32 size2 = s2->get_Length();
    System::Int32 size3 = s3->get_Length();
    System::Int32 size4 = s4->get_Length();
    System::Int32 bufferSize = size1 + size2 + size3 + size4 + 1;   // +1 for the trailing '\0'
    System::String * concatString = __Create__(bufferSize);
    System::Char * buffer = const_cast<System::Char *>(concatString->__ToCString__());
    wmemcpy(buffer, s1->__ToCString__(), size1);
    wmemcpy(buffer + size1, s2->__ToCString__(), size2);
    wmemcpy(buffer + (size1 + size2), s3->__ToCString__(), size3);
    wmemcpy(buffer + (size1 + size2 + size3), s4->__ToCString__(), size4 + 1);
    return (concatString);
}

System::Int32 String::GetHashCode()
{
    // This is _NOT_ a good C++ practices to test directly is this == NULL in a method (given it's non static)
    // But this is actually perfectly valid and we actually use this trick to handle switch on strings, with a case null:
    // As C# enables this case statement!
    if (this == NULL)
    {
        // If this is NULL, returns 0 which is the only non authorized hashcode for strings.
        // As 0 indicate that the hashvalue has not been calculated yet...
        // But here that's a good way to tell that we have a null string.
        return (0);
    }
    int hashCode = mHashCode;
    if (hashCode != 0)
    {
        return (hashCode);
    }

    // 0, means that we have to calculate it the hash-code...
    hashCode = CrossNetRuntime::GetHashCodeForString(mBuffer, mLength);
    if (hashCode == 0)
    {
        // In the case it is zero, we actually use another value so next time we don't recalculate the hash-code
        hashCode = 1;
    }
    mHashCode = hashCode;
    return (hashCode);
}

int String::__GetVariableSize__()
{
    int size = sizeof(String);
    size += (sizeof(System::Char) * (mLength + 1));
    return (size);
}

int String::Compare(const String * a, const String * b)
{
    if (a == b)
    {
        // Handle at the same time, same pointer and a = NULL and b = NULL
        return (0);
    }
    else if (a == NULL)
    {
        // a is NULL, b is not, a < b
        return (-1);
    }
    else if (b == NULL)
    {
        // a is not NULL, b is NULL, a > b
        return (1);
    }
    // So here a and b are not NULL
    return (wcscmp(a->__ToCString__(), b->__ToCString__()));
}

System::Int32 String::Compare(const System::String * a, const System::String * b, bool ignoreCase)
{
    if (a == b)
    {
        // Handle at the same time, same pointer and a = NULL and b = NULL
        return (0);
    }
    else if (a == NULL)
    {
        // a is NULL, b is not, a < b
        return (-1);
    }
    else if (b == NULL)
    {
        // a is not NULL, b is NULL, a > b
        return (1);
    }

    if (ignoreCase)
    {
        return (_wcsicmp(a->__ToCString__(), b->__ToCString__()));
    }
    else
    {
        return (wcscmp(a->__ToCString__(), b->__ToCString__()));
    }
}

System::Int32 String::Compare(const String * /*a*/, const String * /*b*/, System::StringComparison /*comparisonType*/)
{
/*
    switch (comparisonType)
    {
    case StringComparison::CurrentCulture:
    case StringComparison::InvariantCulture:
    case StringComparison::Ordinal:
        return (Compare(a, b));

    default:
        return (Compare(a, b, true));
    }
*/
    CROSSNET_FAIL("");
    return (0);
}

System::Boolean String::Equals(System::Object * obj)
{
    System::String * other = CrossNetRuntime::AsCast<System::String>(obj);
    if (other == NULL)
    {
        return (false);
    }
    return (op_Equality(this, other));
}

System::Int32 String::CompareTo(System::Object * value)
{
    System::String * other = CrossNetRuntime::AsCast<System::String>(value);
    if (other == NULL)
    {
        return (+1);
    }
    return (wcscmp(__ToCString__(), other->__ToCString__()));
}

System::String * String::Trim()
{
    Int32   length = mLength;
    Char *  start = mBuffer;
    Char *  end = mBuffer + length;

    // First skip all the start
    while (start < end)
    {
        Char c = *start;
        switch (c)
        {
        case 0x0009:
        case 0x000a:
        case 0x000b:
        case 0x000c:
        case 0x000d:
        case 0x0020:
            ++start;
            continue;
        }

        // It's not a white space, stop this loop...
        break;
    }

    // Then skip the end (knowing that start maybe already reached end...
    while (start < end)
    {
        --end;
        Char c = *end;
        switch (c)
        {
        case 0x0009:
        case 0x000a:
        case 0x000b:
        case 0x000c:
        case 0x000d:
        case 0x0020:
            continue;
        }

        // It's not a white space, stop this loop...
        ++end;      // Readjust the end (so it won't be part of the string)
        break;
    }

    // So the new string is between start and end (char on end is not included)

    if ((start == mBuffer) && (end == mBuffer + length))
    {
        // Didn't trim anything... Return the same string
        return (this);
    }

    // We trim something, recreate a new string...
    Int32 startIndex = (Int32)(start - mBuffer);
    Int32 size = (Int32)(end - start);
    return (__Create__(mBuffer, startIndex, size));
}

System::Int32 String::IndexOf(System::Char c)
{
    // Because of possible character zero, we cannot use standard C function here
    System::Char * pointer = mBuffer;
    System::Char * endBuffer = mBuffer + mLength;
    while (pointer < endBuffer)
    {
        if (*pointer == c)
        {
            return (pointer - mBuffer);
        }
        ++pointer;
    }
    return (-1);
}

System::Array__G<System::String *> * String::Split(System::Array__G<wchar_t> * array)
{
    System::Array__G<System::String *> * result;
    int arrayLength = array->get_Length();
    if (arrayLength == 0)
    {
        // In case the array of char is empty, just return an array with 1 item...
        result = System::Array__G<System::String *>::__Create__(1);
        result->Item(0) = this;
        return (result);
    }

    int i, j, length;
    length = mLength;

    // First count the number of strings to create...
    int numberOfSplits = 0;
    for (i = 0 ; i < length ; ++i)
    {
        System::Char c = mBuffer[i];

        for (j = 0 ; j < arrayLength ; ++j)
        {
            System::Char pattern = array->Item(j);
            if (c == pattern)
            {
                ++numberOfSplits;
                break;
            }
        }
    }

    if (numberOfSplits == 0)
    {
        // The patterns did not split anything...
        // We are roughly in the same case as if there were no pattern
        result = System::Array__G<System::String *>::__Create__(1);
        result->Item(0) = this;
        return (result);
    }

    // Now that the number of splist is determined, let's create the corresponding array
    result = System::Array__G<System::String *>::__Create__(numberOfSplits + 1);
    int stringStart = 0; 
    int currentStringIndex = 0;
    for (i = 0 ; i < length ; ++i)
    {
        System::Char c = mBuffer[i];

        for (j = 0 ; j < arrayLength ; ++j)
        {
            System::Char pattern = array->Item(j);
            if (c == pattern)
            {
                int localLength = i - stringStart;
                System::String * newString = __Create__(mBuffer, stringStart, localLength);
                // Put the string and increment the counter
                result->Item(currentStringIndex++) = newString;
                stringStart = i + 1; // Skip the pattern
                break;
            }
        }
    }

    // And add the last string...
    {
        int localLength = length - stringStart;
        System::String * newString = String::__Create__(mBuffer, stringStart, localLength);
        // Put the string
        result->Item(currentStringIndex++) = newString;
    }

    return (result);
}

System::String * String::Format(System::String * format, System::Object * arg0)
{
    System::String * string0 = NULL;    // By default the string is not initialized

    int i, length;
    const System::Char * formatBuffer = format->__ToCString__();
    length = format->get_Length();
    System::Text::StringBuilder * strBuilder = System::Text::StringBuilder::__Create__(2 * length);

    // TODO: Optimize this code...
    // TODO: Improve parsing and error detection...
    bool withinBraces = false;
    const System::Char * afterStartBraces = NULL;
    for (i = 0 ; i < length ; ++i)
    {
        System::Char c;
        c = *formatBuffer++;
        if (withinBraces)
        {
            if (c != L'}')
            {
                // Do nothing here...
            }
            else
            {
                withinBraces = false;

                // Now we can evaluate the value between { and }
                const System::Char * beforeEndBraces =  formatBuffer - 2;   // Character just before the brace
                beforeEndBraces;
                // Because that's a Format function with just one parameter, only 0 is expected
                CROSSNET_ASSERT(afterStartBraces == beforeEndBraces, "");
                CROSSNET_ASSERT(*afterStartBraces == L'0', "");

                if (string0 == NULL)
                {
                    // We did not get the string yet, get it only one time,
                    // even if {0} is referenced several times...
                    string0 = arg0->ToString();
                }

                // Replace "{0}" by the stringified arg0
                strBuilder->Append(string0);
            }
        }
        else
        {
            if (c != L'{')
            {
                strBuilder->Append(c);
            }
            else
            {
                withinBraces = true;
                afterStartBraces = formatBuffer;
            }
        }
    }
    return (strBuilder->ToString());
}

System::String * String::Concat(System::Array__G<System::Object *> * array)
{
    System::Int32 totalSize = 1;
    int arrayLength = array->get_Length();

    // Reserve room for the string pointers
    System::String * * allStrs = (System::String * *)_alloca(arrayLength * sizeof(System::String *));

    // Get all the strings, stores them in a temporary array and calculate the total length
    for (int i = 0 ; i < arrayLength ; ++i)
    {
        System::Object * obj = array->SingleDimensionItem(i);
        System::String * str = obj->ToString();
        allStrs[i] = str;
        totalSize += str->get_Length();
    }

    // Now fill the buffer...
    System::String * concatString = __Create__(totalSize);
    System::Char * buffer = const_cast<System::Char *>(concatString->__ToCString__());
    int offset = 0;
    for (int i = 0 ; i < arrayLength ; ++i)
    {
        System::String * str = allStrs[i];
        wcscpy_s(buffer + offset, totalSize - offset, str->__ToCString__());
        offset += str->get_Length();
    }

    CROSSNET_ASSERT(offset + 1 == totalSize, "");

    // allStrs is deallocated at the end of this method...
    return (concatString);
}

System::String * String::Concat(System::Array__G<System::String *> * array)
{
    System::Int32 totalSize = 1;
    int arrayLength = array->get_Length();

    // Calculate the total length
    for (int i = 0 ; i < arrayLength ; ++i)
    {
        System::String * str = array->SingleDimensionItem(i);
        totalSize += str->get_Length();
    }

    // Now fill the buffer...
    System::String * concatString = __Create__(totalSize);
    System::Char * buffer = const_cast<System::Char *>(concatString->__ToCString__());
    int offset = 0;
    for (int i = 0 ; i < arrayLength ; ++i)
    {
        System::String * str = array->SingleDimensionItem(i);
        wcscpy_s(buffer + offset, totalSize - offset, str->__ToCString__());
        offset += str->get_Length();
    }

    CROSSNET_ASSERT(offset + 1 == totalSize, "");

    return (concatString);
}

System::Boolean String::Contains(String * value)
{
    value;
    CROSSNET_NOT_IMPLEMENTED();
    return (false);
}

}