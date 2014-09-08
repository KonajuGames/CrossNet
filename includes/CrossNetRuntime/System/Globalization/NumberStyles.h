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

#ifndef __SYSTEM_GLOBALIZATION_NUMBERSTYLES_H__
#define __SYSTEM_GLOBALIZATION_NUMBERSTYLES_H__

// The user cannot override this type
#include "CrossNetRuntime/Internal/EnumWrapper.h"

namespace System
{
    namespace Globalization
    {
        BEGIN_DECLARE_ENUM(NumberStyles, ::System::Int32, 0)
            static const ::System::Int32 None = 0;
            static const ::System::Int32 AllowLeadingWhite = 1;
            static const ::System::Int32 AllowTrailingWhite = 2;
            static const ::System::Int32 AllowLeadingSign = 4;
            static const ::System::Int32 AllowTrailingSign = 8;
            static const ::System::Int32 AllowParentheses = 16;
            static const ::System::Int32 AllowDecimalPoint = 32;
            static const ::System::Int32 AllowThousands = 64;
            static const ::System::Int32 AllowExponent = 128;
            static const ::System::Int32 AllowCurrencySymbol = 256;
            static const ::System::Int32 AllowHexSpecifier = 512;
            static const ::System::Int32 Integer = 7;
            static const ::System::Int32 HexNumber = 515;
            static const ::System::Int32 Number = 111;
            static const ::System::Int32 Float = 167;
            static const ::System::Int32 Currency = 383;
            static const ::System::Int32 Any = 511;
        END_DECLARE_ENUM
    }
}

#endif