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

#ifndef __GCALLOCATOR_H__
#define	__GCALLOCATOR_H__

#include "CrossNetRuntime/Defines.h"
#include "CrossNetRuntime/InitOptions.h"

namespace CrossNetRuntime
{
    class GCAllocator
    {
    public:
        static void Setup(const ::CrossNetRuntime::InitOptions & options);
        static void Teardown();

        // We might want to add some aligned allocation as well
        //  (might be needed for VMX code for example).

//  Define this macro if you want to override it in your code
//  #define CN_GC_NO_DEFAULT_ALLOCATE_FREE_IMPLEMENTATION
        static void *   Allocate(int size);
        static void     Free(void * freedPtr, int size);

//  #define CN_GC_NO_UNMANAGED_ALLOCATE_FREE_IMPLEMENTATION
        static void *   UnmanagedAllocate(int size);
        static void     UnmanagedFree(int size);

    private:
        CROSSNET_FINLINE
        static int NextPowerOf2(int size)
        {
            --size;
            size |= (size >> 16);
            size |= (size >> 8);
            size |= (size >> 4);
            size |= (size >> 2);
            size |= (size >> 1);
            ++size;
            return (size);
        }

        CROSSNET_FINLINE
        static int TopBit(int size)
        {
            // TODO:    This implementation is using a slow loop, we'll optimize that later!
            int topBit = 0;
            while (size != 0)
            {
                size >>= 1;
                ++topBit;
            }
            return (topBit);
        }

        CROSSNET_FINLINE
        static int Align(int value)
        {
            value += ALIGNMENT - 1;
            value &= -ALIGNMENT;
            return (value);
        }

        CROSSNET_FINLINE
        static bool IsAligned(int value)
        {
            return ((value & (ALIGNMENT - 1)) == 0);
        }

        CROSSNET_FINLINE
        static bool IsAligned(void * ptr)
        {
            return (((int)(ptr) & (ALIGNMENT - 1)) == 0);
        }

        enum
        {
            // The current alignment is 16 for simplicity reason
            // We could actually have an alignment of 8 with a minimum size of 16
            // Having the 4 lower bits always at zero when doing allocation will help us detect roots
            // With stack crawling...
            ALIGNMENT_SHIFT = 4,
            ALIGNMENT = 1 << ALIGNMENT_SHIFT,

            // Minimum size allocated
            // System.Object takes 12 bytes (4 bytes for the VTable, 4 for the interface map, 4 for the flags).
            MIN_SIZE = 16,

            // All allocations of 1 Kb or less will be allocated with the exact size
            SMALL_SIZE_SHIFT = 10,
            SMALL_SIZE_BIN  = 1 << SMALL_SIZE_SHIFT,

            // Bigger size where we use slow allocator
            // This is currently not used yet...
            //  BIG_SIZE_BIN    = 16 * 1024,

            // Note that in between SMALL_SIZE_BIN and BIG_SIZE_BIN we are using a medium
            // allocator with a slightly different allocator from the small one

            // Marker to tell that the block is free
            // Must be odd to make sure it doesn't correspond to a VTable or interface map
            // In case, VTable would not be the first pointer
            FREE_MARKER = 0xFEEFFEEF,

            // Marker to tell that the block is allocated but still has a size
            // This will be used for arrays / strings for example...
            //  This should not be used...
            SIZED_MARKER = 0x72951413,
        };

        struct AllocStructure
        {
            int                 mMarker;
            AllocStructure *    mNext;
            int                 mSize;
            int                 mPad;
        };

        static void *   Allocate(int size, bool afterGC);
        static void     InternalFree(AllocStructure * freedPtr, int alignedSize);
        static void *   GetCurrentAllocPointer();
        static void     SetCurrentAllocPointer(void * currentPointer);
        static bool     InCurrentAllocationSpace(void * pointer);

        static void     ClearBins();
        static void     ReconcileMediumCache();
        static void     SafeReconcileMediumCache();

        static void *           sEndMainBuffer;
        static unsigned char *  sCurrentAllocPointer;
        static AllocStructure * sSmallBin[SMALL_SIZE_BIN / ALIGNMENT];
        static AllocStructure * sMediumBin[32];
        static AllocStructure * sCurrentMediumPointer;
        static int              sCurrentMediumSize;

        friend class GCManager;
    };
}

#endif

