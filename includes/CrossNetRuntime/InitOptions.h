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

#ifndef __INITOPTIONS_H__
#define __INITOPTIONS_H__

namespace System
{
    class Object;
}

namespace CrossNetRuntime
{
    typedef void *  (*AllocateFunctionPointer)(int size);
    typedef void    (*FreeFunctionPointer)(void * buffer, int size);
    typedef void    (*MasterTraceFunctionPointer)(unsigned char currentMarker);
    // Callback called when a System.Object is destroyed
    // Only used if DESTRUCT_OBJECT_CALLBACK is defined
    typedef void    (*OnDestructObjectPtr)(System::Object * object);

    typedef void *  (*UnmanagedAllocateFunctionPointer)(int size);
    typedef void    (*UnmanagedFreeFunctionPointer)(void * buffer);

    typedef void    (*RegisterSystemTypeFunctionPointer)();

    struct InitOptions
    {
        InitOptions()
        {
            __memclear__(this, sizeof(*this));
        }

        // Interface map
        void *  mInterfaceMapBuffer;
        int     mInterfaceMapSize;
        int     mInitialReservedNumTypes;
        // This gets called as soon as the interface map is registered
        //  But before any CrossNetRuntime type is created
        RegisterSystemTypeFunctionPointer   mRegisterSystemTypeCallback;

        // Allocator

        // Buffer for the main buffer
        void *  mMainBuffer;
        // Size for the main buffer
        int     mMainBufferSize;

        // Design flaw to resolve soon:
        //  If the user allocates some memory, we are actually not able to deallocate it 
        //  By the user callback, the memory will stay allocated...
        //  TO DO:  Improve this and find ways to free them as needed (by a function call?)
        //          And when recycling, we need to try to use first normal memory and avoid to use user provided memory last...
        //          Note actually we cannot collect memory allocated by the user ;)
        //          We really have to improve this...

        // Callback used to allocate when the allocation is full but before GC
        //  The goal is to let the user decide if he prefers use more memory to save GC
        AllocateFunctionPointer     mAllocateBeforeGCCallback;

        // Callback used to allocate when the allocation is full even after GC
        //  If this callback is not implemented or return NULL, the allocation will fail
        AllocateFunctionPointer     mAllocateAfterGCCallback;

        // Callback used to free bigger blocks (or when the allocation was done outside the SOA).
        // Currently not used
        FreeFunctionPointer         mFreeCallback;

        UnmanagedAllocateFunctionPointer    mUnmanagedAllocateCallback;
        UnmanagedFreeFunctionPointer        mUnmanagedFreeCallback;

        // GC
        MasterTraceFunctionPointer  mMainTrace;
        OnDestructObjectPtr         mDestructGCObjectCallback;

    private:
        static InitOptions sOptions;

        friend void Setup(const InitOptions & options);
        friend const InitOptions & GetOptions();
    };

    const InitOptions & GetOptions();
}

#endif

