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

#include "CrossNetRuntime/GC/GCAllocator.h"
#include "CrossNetRuntime/GC/GCManager.h"
#include "CrossNetRuntime/Assert.h"

namespace CrossNetRuntime
{

void *                          GCAllocator::sEndMainBuffer = NULL;
unsigned char *                 GCAllocator::sCurrentAllocPointer = NULL;
GCAllocator::AllocStructure *   GCAllocator::sSmallBin[SMALL_SIZE_BIN / ALIGNMENT];
GCAllocator::AllocStructure *   GCAllocator::sMediumBin[32];
GCAllocator::AllocStructure *   GCAllocator::sCurrentMediumPointer = NULL;
int                             GCAllocator::sCurrentMediumSize = 0;

void GCAllocator::Setup(const ::CrossNetRuntime::InitOptions & options)
{
    CROSSNET_ASSERT(IsAligned(sizeof(AllocStructure)), "");
    CROSSNET_ASSERT(IsAligned((int)options.mMainBuffer), "");

    // Set the allocated buffer to a specific pattern (to detect bugs earlier)
    __memset__(options.mMainBuffer, 0xA5, options.mMainBufferSize);

    sCurrentAllocPointer = static_cast<unsigned char *>(options.mMainBuffer);
    sEndMainBuffer = sCurrentAllocPointer + options.mMainBufferSize;

    ClearBins();
}

void GCAllocator::Teardown()
{
    // For the moment doesn't do anything
    //  We should deallocate user allocated memory here...
}

#ifndef CN_GC_NO_DEFAULT_ALLOCATE
// This allocator has not been overriden by the user, so let's implement it here
void * GCAllocator::Allocate(int size)
{
    return (Allocate(size, false));
}

void * GCAllocator::Allocate(int size, bool afterGC)
{
    AllocStructure *  ptr;

    // Old obsolete comments:

    // Either the small object allocator on the corresponding is empty
    // or we are allocating a bigger size
    // For this implementation, we are not differentiating between medium and bigger size...
    // I.e. 2Kb / 16 Kb / 100 Kb will be allocated on the same buffer
    // We might certainly want to improve this later...

    // Here it means that we reached the end of the buffer
    //  First look at the free block that are sparkled in the buffer
    // Start by the closest size and continue until the biggest
    // We use the next power of 2 to make sure the next block is big enough
    // In reality we should try to have a best fit (that one is in between first fit / best fit)
    // Or at least a better fit ;)


    if (size <= SMALL_SIZE_BIN)
    {
        // Allocation that happens most of the time
        // First look at the small object allocator
        // We need to assume that the small object allocator is filled enough

        // By doing this we reuse memory that has been allocated and freed before
        // This reduces memory consumption and fragmentation as well

/*
        int indexSmallBin = size >> ALIGNMENT_SHIFT;
        ptr = sSmallBin[indexSmallBin];
        if (ptr != NULL)
        {
            sSmallBin[indexSmallBin] = ptr->mNext;
            // Done!
            // Cost:    2 tests, 1 operation, 2 reads, 1 write
            return (ptr);
        }
*/

        int alignedSize = Align(size);

        // Special optimization for small size
        ptr = sCurrentMediumPointer;
        if (ptr != NULL)
        {
            CROSSNET_ASSERT(IsAligned(ptr), "");
            CROSSNET_ASSERT(IsAligned(sCurrentMediumSize), "");

            // Good news, there is already a cached medium size
//          if (sCurrentMediumSize >= size)     // For whatever reason, this is actually crashing... Use the bigger size,
                                                //  this should not change tremendously the performance...
                                                //  TODO:   Investigate this...
            if (sCurrentMediumSize >= SMALL_SIZE_BIN)
            {
                CROSSNET_ASSERT(sCurrentMediumSize >= alignedSize, "");
                // And the buffer is big enough...
                // ptr is going to be the allocated pointer

                // Note that when we use the medium buffer cache, we are not reading / writing anything more
                // I.e. we are not pushing free blocks all over the place, nor poping / pushing in the medium size bin
                // Or even looking iteratively at each medium size bins
                sCurrentMediumPointer = (AllocStructure *)(((unsigned char *)ptr) + alignedSize);
                sCurrentMediumSize -= alignedSize;

                return (ptr);
            }

            // The buffer cannot be used anymore (smaller than the asked size)
            // It certainly is a small size buffer now too ;)

            // Clear the cache and look at the next medium buffer...
            ReconcileMediumCache();
        }

{
    unsigned char * currentAlloc = sCurrentAllocPointer;
    unsigned char * endAlloc = currentAlloc + alignedSize;

    if (endAlloc < sEndMainBuffer)
    {
        // We have enough memory to allocate
        sCurrentAllocPointer = endAlloc;

        // Second most common case (if we are not running out of memory quickly)
        //  Cost if size > SMALL_SIZE_BIN:  2 tests, 2 operations, 1 read, 1 write
        //
        //  Cost if size <= SMALL_SIZE_BIN: 3 tests, 3 operations, 2 reads, 1 write
        return (currentAlloc);
    }
}

        // A bit more optimized for small size...
        int topBit = SMALL_SIZE_SHIFT;
        do
        {
            ptr = sMediumBin[topBit];
            if (ptr != NULL)
            {
                // We found a block that should have enough size...
                break;
            }
            ++topBit;
        }
        while (topBit < 32);

        if (ptr != NULL)
        {
            CROSSNET_ASSERT(ptr->mSize >= size, "");

            // Good, we found a free block of the good size!
            // Remove it from the free list, move the next free block at the top
            sMediumBin[topBit] = ptr->mNext;

            int deltaSize = ptr->mSize - alignedSize;
            CROSSNET_ASSERT(deltaSize >= 0, "");                // The free block should be at least as big as the allocation we are looking for
            CROSSNET_ASSERT(ptr->mSize >= (1 << topBit), "");   // The block should be bigger than the corresponding top bit
            CROSSNET_ASSERT(IsAligned(deltaSize), "");

            // Because we are allocating for a small size and we look inside the medium bin
            // We know that we are going to have left over room...
            // This will increase memory fragmentation

            AllocStructure * newFreeBlock = (AllocStructure *)(((unsigned char *)ptr) + alignedSize);

            // Put this medium buffer in the cache for next time...
            sCurrentMediumPointer = newFreeBlock;
            sCurrentMediumSize = deltaSize;

            // No need to free the block...
            return (ptr);
        }
    }
    else
    {
        // Bigger than small size bin
        int alignedSize = Align(size);

{

    unsigned char * currentAlloc = sCurrentAllocPointer;
    unsigned char * endAlloc = currentAlloc + alignedSize;

    if (endAlloc < sEndMainBuffer)
    {
        // We have enough memory to allocate
        sCurrentAllocPointer = endAlloc;

        // Second most common case (if we are not running out of memory quickly)
        //  Cost if size > SMALL_SIZE_BIN:  2 tests, 2 operations, 1 read, 1 write
        //
        //  Cost if size <= SMALL_SIZE_BIN: 3 tests, 3 operations, 2 reads, 1 write
        return (currentAlloc);
    }
}

        // In that case we have to update the medium bin with the cached medium pointer (if there is one)
        SafeReconcileMediumCache();

        int topBit = TopBit(NextPowerOf2(size));
        do
        {
            ptr = sMediumBin[topBit];
            if (ptr != NULL)
            {
                // We found a block that should have enough size...
                break;
            }
            ++topBit;
        }
        while (topBit < 32);

        if (ptr != NULL)
        {
            CROSSNET_ASSERT(ptr->mSize >= size, "");

            // Good, we found a free block of the good size!
            // Remove it from the free list, move the next free block at the top
            sMediumBin[topBit] = ptr->mNext;

            int deltaSize = ptr->mSize - alignedSize;
            CROSSNET_ASSERT(deltaSize >= 0, "");                // The free block should be at least as big as the allocation we are looking for
            CROSSNET_ASSERT(ptr->mSize >= (1 << topBit), "");   // The block should be bigger than the corresponding top bit
            CROSSNET_ASSERT(IsAligned(deltaSize), "");

            if (deltaSize > 0)
            {
                // It means that there is some left over from the block
                // We either have to push it on the small bin or on the medium bin
                // This will increase memory fragmentation

                AllocStructure * newFreeBlock = (AllocStructure *)(((unsigned char *)ptr) + alignedSize);
                InternalFree(newFreeBlock, deltaSize);
            }
            return (ptr);
        }
    }
/*
    {
        unsigned char * currentAlloc = sCurrentAllocPointer;
        unsigned char * endAlloc = currentAlloc + alignedSize;

        if (endAlloc < sEndMainBuffer)
        {
            // We have enough memory to allocate
            sCurrentAllocPointer = endAlloc;

            // Second most common case (if we are not running out of memory quickly)
            //  Cost if size > SMALL_SIZE_BIN:  2 tests, 2 operations, 1 read, 1 write
            //
            //  Cost if size <= SMALL_SIZE_BIN: 3 tests, 3 operations, 2 reads, 1 write
            return (currentAlloc);
        }
    }
*/

    // Let's recapitulate, we did not find a free block in:
    //  1. The Small Object Allocator (recycled small objects)
    //  2. At the end of the main buffer (for new objects)
    //  3. In the medium allocator (recycled medium and big objects).

    if (afterGC)
    {
        // We did a GC already with no luck...
        // let's try with the last user allocator
        AllocateFunctionPointer func = ::CrossNetRuntime::GetOptions().mAllocateAfterGCCallback;
        if (func != NULL)
        {
            return (func(size));
        }
        // No function pointer set, can't allocate
        return (NULL);
    }
    else
    {
        // Before we collect, ask the user what he wants to do
        AllocateFunctionPointer func = ::CrossNetRuntime::GetOptions().mAllocateBeforeGCCallback;
        if (func != NULL)
        {
            // He provided a callback, let's use it
            void * result = func(size);
            if (result != NULL)
            {
                // And the callback allocated something!
                return (result);
            }
        }
    }

    CROSSNET_ASSERT(afterGC == false, "Can only before collect here!");

    // So we are before collect and everything failed,
    //  Even the user didn't provide an allocation or were not able to allocate more

    // The only remaining thing to do is to Garbage Collect,
    // hoping it will free some memory...

    GCManager::Collect(GCManager::MAX_GENERATION, false);

    // Recurse the same function again, this time stating that the GC has been done already
    // This won't be done more often...
    return (Allocate(size, true));
}

// As you can see, we can free a single block of memory at any time without being inside a GC
void GCAllocator::Free(void * ptr, int size)
{
    CROSSNET_ASSERT(IsAligned(ptr), "");

    AllocStructure * freedPtr = static_cast<AllocStructure *>(ptr);
    int alignedSize = Align(size);
    InternalFree(freedPtr, alignedSize);
}

void    GCAllocator::InternalFree(AllocStructure * freedPtr, int alignedSize)
{
    CROSSNET_ASSERT(IsAligned(freedPtr), "");
    CROSSNET_ASSERT(IsAligned(alignedSize), "");

    freedPtr->mMarker = FREE_MARKER;
    freedPtr->mSize = alignedSize;

/*
    if (alignedSize <= SMALL_SIZE_BIN)
    {
        // Deallocation that happens most of the time
        int indexSmallBin = alignedSize >> ALIGNMENT_SHIFT;
        freedPtr->mNext = sSmallBin[indexSmallBin];
        sSmallBin[indexSmallBin] = freedPtr;
    }
    else
*/
    {
        // Do not use next power of 2 here, as this should be greater than the size we are looking for
        int newTopBit = TopBit(alignedSize + 1);        // Increment so if the size is exactly a poer of two, the bit will stay in the same range
        --newTopBit;                                    // But will write one level below as the size is guaranteed to be at least in the range we are looking

        CROSSNET_ASSERT(alignedSize >= (1 << newTopBit), "");       // Check that the range is correct...
        CROSSNET_ASSERT(alignedSize < (1 << (newTopBit + 1)), "");

        freedPtr->mNext = sMediumBin[newTopBit];
        sMediumBin[newTopBit] = freedPtr;
    }
}

#endif

void * GCAllocator::GetCurrentAllocPointer()
{
    return (sCurrentAllocPointer);
}

void    GCAllocator::SetCurrentAllocPointer(void * currentPointer)
{
    sCurrentAllocPointer = (unsigned char *)currentPointer;
}

bool    GCAllocator::InCurrentAllocationSpace(void * pointer)
{
    if (pointer < GetOptions().mMainBuffer)
    {
        // before the main buffer
        return (false);
    }

    if (pointer >= sCurrentAllocPointer)
    {
        // after the allocated buffer
        return (false);
    }

    return (true);
}

void   GCAllocator::ClearBins()
{
    for (int i = 0 ; i < sizeof(sSmallBin) / sizeof(sSmallBin[0]) ; ++i)
    {
        sSmallBin[i] = NULL;
    }

    for (int i = 0 ; i < sizeof(sMediumBin) / sizeof(sMediumBin[0]) ; ++i)
    {
        sMediumBin[i] = NULL;
    }
}

void    GCAllocator::ReconcileMediumCache()
{
    // Simply clear the cache so now the cached state and the memory state are reconciled
    if (sCurrentMediumSize != 0)
    {
        InternalFree(sCurrentMediumPointer, sCurrentMediumSize);
        sCurrentMediumSize = 0;
    }
    sCurrentMediumPointer = NULL;
}

void    GCAllocator::SafeReconcileMediumCache()
{
    if (sCurrentMediumPointer != NULL)
    {
        ReconcileMediumCache();
    }
}

}

