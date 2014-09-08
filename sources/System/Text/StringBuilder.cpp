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

#include "CrossNetRuntime/System/Text/StringBuilder.h"
#include "CrossNetRuntime/System/String.h"

namespace System
{
    namespace Text
    {

void * * StringBuilder::s__InterfaceMap__ = NULL;

void StringBuilder::__RegisterId__()
{
    s__InterfaceMap__ = CrossNetRuntime::InterfaceMapper::RegisterObject(sizeof(System::Text::StringBuilder));
}

StringBuilder::StringBuilder()
    :
    mSize(0),
    mCapacity(0),
    mBuffer(NULL)
{
    m__InterfaceMap__ = __GetInterfaceMap__();
}

StringBuilder * StringBuilder::__Create__()
{
    StringBuilder * temp = new StringBuilder();
    return (temp);
}

StringBuilder * StringBuilder::__Create__(System::Int32 capacity)
{
    StringBuilder * temp = new StringBuilder();
    temp->Reserve(capacity);
    return (temp);
}

System::String *    StringBuilder::ToString()
{
    if (mSize == 0)
    {
        return (System::String::Empty);
    }
    return (String::__Create__(mBuffer, 0, mSize));
}

StringBuilder * StringBuilder::Append(System::String * text)
{
    int stringLength = text->get_Length();
    int newSize = mSize + stringLength + 1;     // +1 for the trailing '\0'
    if (newSize > mCapacity)
    {
        Reserve(newSize);
    }
    // Copy the string and the trailing '\0' as well
    wmemcpy(mBuffer + mSize, text->__ToCString__(), stringLength + 1);
    mSize += stringLength;
    CROSSNET_ASSERT(mSize + 1 <= mCapacity, "");
    return (this);
}

StringBuilder * StringBuilder::Append(System::Char c)
{
    int newSize = mSize + 1 + 1;                // +1 for the char and +1 for the trailing '\0'
    if (newSize > mCapacity)
    {
        Reserve(newSize);
    }
    mBuffer[mSize] = c;
    ++mSize;
    mBuffer[mSize] = L'\0';
    CROSSNET_ASSERT(mSize + 1 <= mCapacity, "");
    return (this);
}

void    StringBuilder::Reserve(System::Int32 newSize)
{
    if (newSize <= mCapacity)
    {
        // Enough size already...
        return;
    }

    if (mCapacity == 0)
    {
        // This is actually the first allocation through the StringBuilder
        // The allocation doesn't have to copy from the previous buffer
        CROSSNET_ASSERT(mSize == 0, "");
        CROSSNET_ASSERT(mBuffer == NULL, "");

        if (newSize < MIN_CAPACITY)
        {
            newSize = MIN_CAPACITY;
        }
        
        mCapacity = newSize;
        // mSize is 0 already
        mBuffer = new System::Char[newSize];
        return;
    }

    // if we grow, at minimum grow by 25%
    int incrementalCapacity = (mCapacity * 10) / 8;
    if (incrementalCapacity > newSize)
    {
        newSize = incrementalCapacity;
    }

    System::Char * newBuffer = new System::Char[newSize];
    // Copy the whole buffer and the trailing '\0'
    wmemcpy(newBuffer, mBuffer, mSize + 1);

    delete[] mBuffer;
    mBuffer = newBuffer;
    mCapacity = newSize;
}

}
}