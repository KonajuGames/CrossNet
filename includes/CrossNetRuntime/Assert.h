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

#ifndef __CROSSNET_ASSERT_H__
#define	__CROSSNET_ASSERT_H__

// The current implementations of the assert is actually a crash...
// This is on purpose to make sure we don't overlook any issue...

#if _DEBUG

#define CROSSNET_FAIL(a)            __asm { int 3   }
#define CROSSNET_ASSERT(a, b)       if ((a) == false)   {   __asm { int 3   } }
#define CROSSNET_FATAL(a, b)        if ((a) == false)   {   __asm { int 3   } }
#define CROSSNET_NOT_IMPLEMENTED()  __asm { int 3   }
#define CROSSNET_DONT_CALL()        __asm { int 3   }

#else

#define CROSSNET_FAIL(a)            {}
#define CROSSNET_ASSERT(a, b)       {}
#define CROSSNET_FATAL(a, b)        {}
#define CROSSNET_NOT_IMPLEMENTED()  {}
#define CROSSNET_DONT_CALL()        {}

#endif

#endif	// __CROSSNET_ASSERT_H__