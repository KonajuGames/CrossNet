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

#ifndef __CROSSNET_DEFINES_H__
#define __CROSSNET_DEFINES_H__

// For memset, memcpy
// Even if that's within a define and we don't need it right away, the header is included so we can now use the macro
#include <memory.h>

#define CROSSNET_FINLINE    __forceinline
#define CROSSNET_INLINE     inline

#define CROSSNET_STRINGIFY2(a, b)    a ## b
#define CROSSNET_STRINGIFY3(a, b, c) a ## b ## c

// Call the callback when deleting object
//  The goal is to make sure we are always deleting an object during a collection
//  Later when the GC will be optimized, this is going to be overriden...
#define DESTRUCT_GCOBJECT_CALLBACK

#ifndef __stackalloc__
#define __stackalloc__(size)            (memset(_alloca(size), 0, size))
#endif

#ifndef __memclear__
#define __memclear__(ptr, size)         (memset(ptr, 0, size))
#endif

#ifndef __memset__
#define __memset__(ptr, pattern, size)  (memset(ptr, pattern, size))
#endif

#ifndef __memcopy__
#define __memcopy__(dst, src, size)     (memcpy(dst, src, size))
#endif

#ifndef __memcmp__
#define __memcmp__(ptr1, ptr2, size)    (memcmp(ptr1, ptr2, size))
#endif

// Few macros to wrap parameters for sub-macros ;)
// We could use variadic macros but not all C++ compilers are implementing that...

// Oh well... wrap up to 20 parameters...
// As a side note, after 20, you can still have wrapper within wrapper anyway (although the parser would have to do more work to use it). 

// I'm sure there are a bunch of ways to do that, but I'm just not aware of...

#define __W0__()
#define __W1__(a)                                                                   a
#define __W2__(a, b)                                                                a, b
#define __W3__(a, b, c)                                                             a, b, c
#define __W4__(a, b, c, d)                                                          a, b, c, d
#define __W5__(a, b, c, d, e)                                                       a, b, c, d, e
#define __W6__(a, b, c, d, e, f)                                                    a, b, c, d, e, f
#define __W7__(a, b, c, d, e, f, g)                                                 a, b, c, d, e, f, g
#define __W8__(a, b, c, d, e, f, g, h)                                              a, b, c, d, e, f, g, h
#define __W9__(a, b, c, d, e, f, g, h, i)                                           a, b, c, d, e, f, g, h, i
#define __W10__(a, b, c, d, e, f, g, h, i, j)                                       a, b, c, d, e, f, g, h, i, j
#define __W11__(a, b, c, d, e, f, g, h, i, j, k)                                    a, b, c, d, e, f, g, h, i, j, k
#define __W12__(a, b, c, d, e, f, g, h, i, j, k, l)                                 a, b, c, d, e, f, g, h, i, j, k, l
#define __W13__(a, b, c, d, e, f, g, h, i, j, k, l, m)                              a, b, c, d, e, f, g, h, i, j, k, l, m
#define __W14__(a, b, c, d, e, f, g, h, i, j, k, l, m, n)                           a, b, c, d, e, f, g, h, i, j, k, l, m, n
#define __W15__(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)                        a, b, c, d, e, f, g, h, i, j, k, l, m, n, o
#define __W16__(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)                     a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p
#define __W17__(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)                  a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q
#define __W18__(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r)               a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r
#define __W19__(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s)            a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s
#define __W20__(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t)         a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t

// Macro trick to directly prefix the parameters with a comma if they are different of __W0__
// If the parameters have been passed by __W0__ then no comma will be added.
// This is to solve the issue of the interface call, instance is always passed
// but there might be no further parameter, so we have to determine if a comma is needed or not...

// The interesting rule I discoved on the MS preprocessor doc is that after ## collapsed the two tokens,
// the resulting token is evaluated for expansion
// That's the reason why __COMMA##__W1__(a) will be collapsed in __COMMA__W1__(a)
// Which then in return will be expanded to , __W1(a), to be finally expanded to , a

// That's interesing trick... There might be other stuff like this that we could do...
// Side note: if the caller of the macro doesn't use __W?__ to pass parameters
// This will result in a compile error as __COMMASomething won't be expanded properly...

#define __COMMA__W0__   __W0__
#define __COMMA__W1__   , __W1__
#define __COMMA__W2__   , __W2__
#define __COMMA__W3__   , __W3__
#define __COMMA__W4__   , __W4__
#define __COMMA__W5__   , __W5__
#define __COMMA__W6__   , __W6__
#define __COMMA__W7__   , __W7__
#define __COMMA__W8__   , __W8__
#define __COMMA__W9__   , __W9__
#define __COMMA__W10__  , __W10__
#define __COMMA__W11__  , __W11__
#define __COMMA__W12__  , __W12__
#define __COMMA__W13__  , __W13__
#define __COMMA__W14__  , __W14__
#define __COMMA__W15__  , __W15__
#define __COMMA__W16__  , __W16__
#define __COMMA__W17__  , __W17__
#define __COMMA__W18__  , __W18__
#define __COMMA__W19__  , __W19__
#define __COMMA__W20__  , __W20__

#endif	//	__CROSSNET_DEFINES_H__
