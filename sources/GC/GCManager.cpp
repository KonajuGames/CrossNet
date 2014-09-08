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

#include "CrossNetRuntime/GC/GCManager.h"
#include "CrossNetRuntime/GC/GCAllocator.h"
#include "CrossNetRuntime/CrossNetRuntime.h"
#include <time.h>

namespace CrossNetRuntime
{

// Initial value for the marker,
//  Don't use MARKER_AT_CREATION otherwise newly created object
//  Will be assumed to be traced already
unsigned char   GCManager::sCurrentMarker = (unsigned char)(~::System::Object::__MARKER_AT_CREATION__);
bool            GCManager::sCollecting = false;
int             GCManager::sNumCollections = 0;
double          GCManager::sNumSecondsInGcManager = 0.0f;
double          GCManager::sNumSecondsInTracingPermanent = 0.0f;
double          GCManager::sNumSecondsInTracingStack = 0.0f;
double          GCManager::sNumSecondsInTracingStatics = 0.0f;
double          GCManager::sNumSecondsInCollect = 0.0f;
void *          GCManager::sTopOfStack = NULL;

void GCManager::Setup(const InitOptions & /*options*/)
{
    // Don't store anything, we'll call GetOptions() as needed
}

void GCManager::Teardown()
{
    // Do one last collect
    Collect(MAX_GENERATION, true);

    // Here we should make sure that no more object is allocated
    //  TODO:   Make sure of that!
}

// Note that this implementation doesn't do Intra-frame yet
//  TODO:   Improve this...
//          Parse the stack and the registers and see what object to not collect
void GCManager::Collect(int /* generation */, bool final)
{
    double diff;
    clock_t startGc = clock();

    // Reconcile the cache and the memory so the collection happen on correct memory buffers
    GCAllocator::SafeReconcileMediumCache();

    // First increase marker and avoid ::System::Object::__MARKER_AT_CREATION__
    unsigned int currentMarker = sCurrentMarker;
    ++currentMarker;
    currentMarker &= 0xff;
    if (currentMarker == ::System::Object::__MARKER_AT_CREATION__)
    {
        // We looped, so do it another time
        ++currentMarker;
        currentMarker &= 0xff;
    }
    sCurrentMarker = (unsigned char)currentMarker;

    // Now the current marker is different from any other marker currently stored in previous managed objects
    //  And it is also different from any newly created object...

    // Now trace all the objects from the roots
    //  The user has to provide a single function to do that
    //  If he doesn't, there is big chance that all the objects will be collected
    if (final == false)
    {
        // Trace all the types registered...
        // And all the static members
        // And all the global strings

        clock_t startTracingPermanent = clock();
        CrossNetRuntime::Trace((unsigned char)currentMarker);
        clock_t endTracingPermanent = clock();
        diff = (double)(endTracingPermanent - startTracingPermanent) / (double)CLOCKS_PER_SEC;
        sNumSecondsInTracingPermanent += diff;

        clock_t startTracingStack = endTracingPermanent;
        // Stack crawling should be implemented here
        TraceStack((unsigned char)currentMarker);
        clock_t endTracingStack = clock();
        diff = (double)(endTracingStack - startTracingStack) / (double)CLOCKS_PER_SEC;
        sNumSecondsInTracingStack += diff;

        // Then call the user provided function
        clock_t startTracingStatics = endTracingStack;
        const InitOptions & options = ::CrossNetRuntime::GetOptions();
        if (options.mMainTrace != NULL)
        {
            options.mMainTrace((unsigned char)currentMarker);
        }
        clock_t endTracingStatics = clock();
        diff = (double)(endTracingStatics - startTracingStatics) / (double)CLOCKS_PER_SEC;
        sNumSecondsInTracingStatics += diff;
    }

    // Then we have to parse every single object and find out which one is not traced yet...
    //  I.e. is marker is different from the currentMarker...

    void * mainBuffer = CrossNetRuntime::GetOptions().mMainBuffer;
    GCAllocator::AllocStructure * ptr = static_cast<GCAllocator::AllocStructure *>(mainBuffer);
    // Nothing has been allocated after the current alloc pointer...
    void * endBuffer = GCAllocator::GetCurrentAllocPointer();

    void * firstFree = NULL;

    sCollecting = true;

    // We are going to consolidate all the free blocks,
    //  the bins won't contain any useful information anymore
    //  Clean them to not have garbage next pointers
    GCAllocator::ClearBins();

    clock_t startInCollect = clock();
    while (ptr < endBuffer)
    {
        if (ptr->mMarker == GCAllocator::FREE_MARKER)
        {
            // Free block, go to the next block...
            if (firstFree == NULL)
            {
                firstFree = ptr;        // Mark it as the first free block of the region
            }
            CROSSNET_ASSERT(GCAllocator::IsAligned(ptr->mSize), "");
            ptr += (ptr->mSize / sizeof(GCAllocator::AllocStructure));
            continue;
        }

        GCAllocator::AllocStructure * nextPtr;

        // ptr points to an allocated block, 2 cases...
        ::System::Object * obj = reinterpret_cast<::System::Object *>(ptr);
        // Assert before the crash so it's clearer what is hapenning
        // Look at the VTable to see what is the actual type
        CROSSNET_ASSERT((void *)(obj->m__InterfaceMap__) != NULL, "The interface map has not been set correctly.");
        CROSSNET_ASSERT((int)(obj->m__InterfaceMap__) != System::Object::__FAKE_INTERFACE_MAP__, "The interface map has not been set correctly.");

        int size;
        if ((obj->m__AllFlags__ & ::System::Object::__DYN_ALLOC__) == 0)
        {
            // Standard allocation, use the interface map to get the size
            size = (int)InterfaceMapper::GetSize(obj->m__InterfaceMap__);
        }
        else
        {
            // Variable size allocations (for arrays and strings)
            size = obj->__GetVariableSize__();
        }
        int alignedSize = GCAllocator::Align(size);
        nextPtr = ptr + (alignedSize / sizeof(GCAllocator::AllocStructure));

        // Now that we have the next pointer, we can see if the collection is needed
        if (obj->__GetMark__() != (unsigned char)currentMarker)
        {
            // The mark is different, it means that we need to collect this object
            obj->__OnCollect__();

            // Now we can free the block, at the same time, we can actually free the previous blocks as well
            if (firstFree == NULL)
            {
                firstFree = obj;        // Mark the block as first free block...
            }
        }
        else
        {
            CROSSNET_ASSERT(final == false, "If final, all objects should be collected!");

            // This block is not free
            if (firstFree != NULL)
            {
                // Set the size for the previous free block
                size = (int)ptr - (int)firstFree;
                GCAllocator::Free(firstFree, size);
                firstFree = NULL;
            }
        }

        // Go to the next object...
        ptr = nextPtr;
    }

    // The pointers should match (otherwise we missed something...)
    CROSSNET_ASSERT(ptr == endBuffer, "");

    // We are done with this loop
    if (firstFree != NULL)
    {
        // And it seems that the last block (or set of block) is actually free!
        // Update the current pointer accordingly (as such enables a little defragmentation)
        GCAllocator::SetCurrentAllocPointer(firstFree);
    }

    clock_t endInCollect = clock();
    diff = (double)(endInCollect - startInCollect) / (double)CLOCKS_PER_SEC;
    sNumSecondsInCollect += diff;

    sCollecting = false;

    ++sNumCollections;

    clock_t endGc = endInCollect;
    diff = (double)(endGc - startGc) / (double)CLOCKS_PER_SEC;
    sNumSecondsInGcManager += diff;
}

void GCManager::CollectOneObject(::System::Object * object)
{
    sCollecting = true;

    // Collect the object
    object->__OnCollect__();

    // Then we need to free the corresponding memory
    int size;
    if ((object->m__AllFlags__ & ::System::Object::__DYN_ALLOC__) == 0)
    {
        // Standard allocation, use the interface map to get the size
        size = (int)InterfaceMapper::GetSize(object->m__InterfaceMap__);
    }
    else
    {
        // Variable size allocations (for arrays and strings)
        size = object->__GetVariableSize__();
    }
    GCAllocator::Free(object, size);

    sCollecting = false;
}

void GCManager::CheckCollecting(::System::Object * /*object*/)
{
    // Make sure that we call this function only when we are collecting
    CROSSNET_ASSERT(sCollecting, "");
}

int GCManager::GetNumCollections()
{
    return (sNumCollections);
}

double GCManager::GetNumSecondsInGcManager()
{
    return (sNumSecondsInGcManager);
}

double GCManager::GetNumSecondsInTracingPermanent()
{
    return (sNumSecondsInTracingPermanent);
}

double GCManager::GetNumSecondsInTracingStack()
{
    return (sNumSecondsInTracingStack);
}

double GCManager::GetNumSecondsInTracingStatics()
{
    return (sNumSecondsInTracingStatics);
}

double GCManager::GetNumSecondsInCollect()
{
    return (sNumSecondsInCollect);
}

void GCManager::SetTopOfStack()
{
    void * _ESP;
    // Platform specific code
    __asm mov _ESP, esp
    // End of platform specific code
    sTopOfStack = _ESP;
}

void GCManager::TraceStack(unsigned char mark)
{
    // Platform specific code
    void * _EAX;
    void * _EBX;
    void * _ECX;
    void * _EDX;
    void * _ESI;
    void * _EDI;
    void * _ESP;
    void * _EBP;

    __asm
    {
        mov _EAX, eax
        mov _EBX, ebx
        mov _ECX, ecx
        mov _EDX, edx
        mov _ESI, esi
        mov _EDI, edi
        mov _ESP, esp
        mov _EBP, ebp
    }

    ValidateRoot2(_EAX, mark);
    ValidateRoot2(_EBX, mark);
    ValidateRoot2(_ECX, mark);
    ValidateRoot2(_EDX, mark);
    ValidateRoot2(_ESI, mark);
    ValidateRoot2(_EDI, mark);

    // With some optimization flags, esp. optimize for speed (and some heavily templated / inlined code),
    // EBP can be used as temporary variable... Trace it just in case...
    // In other cases (like listed above) the value saved on the stack won't be the object per say
    // But the pointer with an offset... There is no way to actually trace that.
    // We have to rely on the compiler to have a good behavior!
    ValidateRoot2(_EBP, mark);
    // End of platform specific code

    if ((void *)_ESP > sTopOfStack)
    {
        CROSSNET_FAIL("Top of stack is not set correctly!");
        return;
    }

    // For each value from _ESP to TopOfStack
    // We are going to check if they are valid roots...

    void * * bottomOfStack = (void * *)_ESP;
    void * * topOfStack = (void * *)sTopOfStack;

    while (bottomOfStack < topOfStack)
    {
        ValidateRoot2(*bottomOfStack++, mark);
    }
}

void GCManager::ValidateRoot2(void * value, unsigned char mark)
{
    bool tryAnother = (ValidateRoot(value, mark) == false);
    if (tryAnother)
    {
        int pointer = (int)value;
        // In some _rare_ cases, especially due to compiler optimizations
        // The root pointer on the stack / or register
        // won't point on the object itself but inside the object
        // For example, if the code later is accessing the members after System::Object
        // Let's try to recover from this...

        // In a perfect world, we would either make sure this never happens
        // Or find the corresponding object (sizeof(System::Object) might not always be enough...)
        pointer -= sizeof(::System::Object);
        pointer &= ~(GCAllocator::ALIGNMENT - 1);

        ValidateRoot((void *)pointer, mark);
    }
}

bool GCManager::ValidateRoot(void * value, unsigned char currentMark)
{
    if (GCAllocator::IsAligned(value) == false)
    {
        // The value is not aligned, it can't point to a managed object
        return (false);
    }
    // It's aligned properly

    if (GCAllocator::InCurrentAllocationSpace(value) == false)
    {
        // The value is not in the allocated memory, it can't point to a managed object
        // No need to try around either
        return (true);
    }
    // It's in the allocated space (so we can now read the memory)

    // vtable should be the first value pointed
    unsigned int vtable = *(unsigned int *)value;
    const int VTABLE_MIN_ADDRESS = 0x10000; // Assume the VTable is never below the first 64 Kb of the address space
                                            // TODO:    Find a better range for the vtable addresses
                                            //          Could be with link directives
    if (vtable < VTABLE_MIN_ADDRESS)
    {
        // Cannot point to a vtable
        return (false);
    }
    // Might point to a vtable
    const int VTABLE_ALIGNMENT = 4;
    if ((vtable & (VTABLE_ALIGNMENT - 1)) != 0)
    {
        // Improper alignment for a VTable
        return (false);
    }
    // VTable properly aligned

    // Interface map should be the second value pointed
    unsigned int interfaceMap = *((unsigned int *)(value) + 1);
    const int INTERFACE_MAP_ALIGNMENT = 4;
    if ((interfaceMap & (INTERFACE_MAP_ALIGNMENT - 1)) != 0)
    {
        // Improper alignment for interface map
        return (false);
    }
    // Interface map properly aligned

    if (InterfaceMapper::InInterfaceMapSpace((void *)interfaceMap) == false)
    {
        // Interface map doesn't point to the interface map buffer
        return (false);
    }
    // Interface map points inside the interface map buffer

    // A lot of conditions succeeded.
    // There is an _extremely_ high possibility that we are actually on a real System::Object

    // There are three cases:
    // 1)
    //      value points to a valid System::Object, then the object will be traced as expected
    //
    // 2)
    //      value points to a value on the stack that unfortunately does point to a System::Object
    //      That means that the a System::Object might be traced where it should have been collected
    //      Although it is annoying, this is not a major issue as the object will be collected later
    //
    // 3)
    //      value points to a value on the stack that doesn't point to a System::Object
    //      Even with the several tests, there is a probability for this to happen
    //      We could add more tests (like compare with allocated objects, or a better definition of the range).
    //      If this case happen, the trace call will crash (as the vtable won't point on the correct space).
    //      We will improve this case over time...

    System::Object * object = (System::Object *)value;
    Trace(object, currentMark);

    // Just traced, don't try around
    return (true);
}

}