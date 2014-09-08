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

#ifndef __CROSSNET_NEWDELETE_H__
#define __CROSSNET_NEWDELETE_H__

// For class type, we can allocate a single value on the heap but not as array
// Array of class type are actually array of pointers
#define CN__NEW_DELETE_OPERATORS_FOR_CLASS_TYPE
/*
#define CN__NEW_DELETE_OPERATORS_FOR_CLASS_TYPE                             \
    private:                                                                \
    void * operator new(size_t size)                                        \
    {                                                                       \
        void * buffer = ::CrossNetRuntime::GCAllocator::Allocate(size);        \
        __memclear__(buffer, size);                                         \
        return (buffer);                                                    \
    }                                                                       \
    void operator delete(void * buffer)                                     \
    {                                                                       \
        CROSSNET_FAIL("");                                                  \
    }                                                                       \
    void * operator new[](size_t size);                                     \
    void operator delete[](void * buffer);
*/

// For value type, we cannot allocate a single value on the heap, but we can as array
// The operators new[] / delete[] are public so classes like array can instantiate them
#define CN__NEW_DELETE_OPERATORS_FOR_VALUE_TYPE                             \
    private:                                                                \
    void * operator new(size_t size);                                       \
    void operator delete(void * buffer);                                    \
    public:                                                                 \
    void * operator new[](size_t size)                                      \
    {                                                                       \
        void * buffer = ::CrossNetRuntime::GCAllocator::Allocate(size);        \
        __memclear__(buffer, size);                                         \
        return (buffer);                                                    \
    }                                                                       \
    void operator delete[](void * /*buffer*/)                               \
    {                                                                       \
        CROSSNET_FAIL("");                                                  \
    }

// For interface, we should not do any allocation
#define CN__NEW_DELETE_OPERATORS_FOR_INTERFACE
/*
// See why the compiler needs implementation for delete(void * buffer)
#define CN__NEW_DELETE_OPERATORS_FOR_INTERFACE                              \
    private:                                                                \
    void * operator new(size_t size);                                       \
    void * operator new[](size_t size);                                     \
    void operator delete(void * buffer) {CROSSNET_FAIL("");   }             \
    void operator delete[](void * buffer);
*/

// Wrappers follow same rules as class types...
// By the way, because the wrappers derive from interfaces, we have to define again the operators
// at the wrapper level otherwise we would have link error due to the interface-like operators

// The code is the same as CN__NEW_DELETE_OPERATORS_FOR_CLASS_TYPE except it is public
// So the owner class can instantiate them
// Also the wrapper code doesn't need to be cleared as the wrappers don't contain any data

#define CN__NEW_DELETE_OPERATORS_FOR_WRAPPER

/*
#define CN__NEW_DELETE_OPERATORS_FOR_WRAPPER                                \
    public:                                                                 \
    void * operator new(size_t size)                                        \
    {                                                                       \
        void * buffer = ::CrossNetRuntime::GCAllocator::Allocate(size);        \
        return (buffer);                                                    \
    }                                                                       \
    void operator delete(void * buffer)                                     \
    {                                                                       \
        CROSSNET_FAIL("");                                                  \
        ::CrossNetRuntime::GCAllocator::Free(buffer);                          \
    }                                                                       \
    private:                                                                \
    void * operator new[](size_t size);                                     \
    void operator delete[](void * buffer);
*/

#endif
