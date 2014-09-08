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

#include "CrossNetRuntime/InterfaceMapper.h"
#include "CrossNetRuntime/Assert.h"
#include "CrossNetRuntime/System/Object.h"
#include "CrossNetRuntime/GC/GCManager.h"
#include "CrossNetRuntime/Internal/BaseTypes.h"
#include <memory.h>

namespace CrossNetRuntime
{

void * *    InterfaceMapper::sInterfaceMap = NULL;
int         InterfaceMapper::sInterfaceMapSize = 0;
void * *    InterfaceMapper::sNextFreeSlot= NULL;

// System.Object ID is 0
// System.ValueType ID is -1

// Interfaces start with 1, and are strictly positive
int         InterfaceMapper::sNextInterfaceId = 1;
// Objects start with -2, and are strictly negative
int         InterfaceMapper::sNextObjectId = -2;
void *      InterfaceMapper::sLastInterfaceMap = NULL;

std::vector<int> InterfaceMapper::sStaticInterfaceId;
std::vector<int> InterfaceMapper::sStaticObjectId;

std::vector<::System::Type *> InterfaceMapper::sAllTypes;

void InterfaceMapper::Setup(const ::CrossNetRuntime::InitOptions & options)
{
    // Instead we might want to allocate by smaller size and maybe several times...
    // One thing to worry about is if the IID grows very big...

    int interfaceMapSize = options.mInterfaceMapSize;
    CROSSNET_ASSERT((interfaceMapSize & sizeof(void *)) == 0, "");
    interfaceMapSize &= -(int)(sizeof(void *));             // Make sure the size is aligned on 4 bytes
    sInterfaceMap = (void * *)(options.mInterfaceMapBuffer);
    __memclear__(sInterfaceMap, interfaceMapSize);
    sInterfaceMapSize = interfaceMapSize / sizeof(void *);
    sNextFreeSlot = sInterfaceMap;
    sAllTypes.reserve(options.mInitialReservedNumTypes);

    options.mRegisterSystemTypeCallback();
}

void InterfaceMapper::Teardown()
{
    sInterfaceMap = NULL;
    sAllTypes.clear();
}

void InterfaceMapper::Trace(unsigned char currentMark)
{
    std::vector<::System::Type *>::iterator it, itEnd;

    it = sAllTypes.begin();
    itEnd = sAllTypes.end();

    while (it != itEnd)
    {
        TraceSystemType(*it, currentMark);
        ++it;
    }
}

void * * InterfaceMapper::RegisterInterfaceStaticId(int staticId, InterfaceInfo * info, int numInterfaceInfos)
{
    CROSSNET_ASSERT(staticId > 0, "The interface ID should be strictly positive!");
    // Make sure the static Id is unique
    std::vector<int>::iterator itBegin = sStaticInterfaceId.begin();
    std::vector<int>::iterator itEnd = sStaticInterfaceId.end();
    for (std::vector<int>::iterator it = itBegin ; it != itEnd ; ++it)
    {
        CROSSNET_ASSERT(*it != staticId, "The static Id has been already registered!");
    }
    sStaticInterfaceId.push_back(staticId);

    // We added the static Id, and updated the dynamic Id accordingly...
    System::Type * type = CreateSystemType();
    sAllTypes.push_back(type);
    return (CreateInterfaceMap(type, staticId, 0, info, numInterfaceInfos, NULL));
}

void * * InterfaceMapper::RegisterObjectStaticId(int staticId, size_t size, InterfaceInfo * info, int numInterfaceInfos, void * * parentInterfaceMap)
{
    CROSSNET_ASSERT(staticId <= 0, "The object ID should be negative!");
    // Make sure the static Id is unique
    std::vector<int>::iterator itBegin = sStaticObjectId.begin();
    std::vector<int>::iterator itEnd = sStaticObjectId.end();
    for (std::vector<int>::iterator it = itBegin ; it != itEnd ; ++it)
    {
        CROSSNET_ASSERT(*it != staticId, "The static Id has been already registered!");
    }
    sStaticObjectId.push_back(staticId);

    // We added the static Id, and updated the dynamic Id accordingly...
    System::Type * type = CreateSystemType();
    sAllTypes.push_back(type);
    return (CreateInterfaceMap(type, staticId, size, info, numInterfaceInfos, parentInterfaceMap));
}

void * * InterfaceMapper::RegisterInterface(InterfaceInfo * info, int numInterfaceInfos)
{
    int id = RetrieveNextInterfaceId();
    System::Type * type = CreateSystemType();
    sAllTypes.push_back(type);
    return (CreateInterfaceMap(type, id, 0, info, numInterfaceInfos, NULL));
}

void * * InterfaceMapper::RegisterObject(size_t size, InterfaceInfo * info, int numInterfaceInfos, void * * parentInterfaceMap)
{
    int id = RetrieveNextObjectId();
    System::Type * type = CreateSystemType();
    sAllTypes.push_back(type);
    return (CreateInterfaceMap(type, id, size, info, numInterfaceInfos, parentInterfaceMap));
}

void * * InterfaceMapper::CreateInterfaceMap(System::Type * type, int id, size_t size, CrossNetRuntime::InterfaceInfo * info, int numInterfaceInfos, void * * parentInterfaceMap)
{
    if (id == 0)
    {
        // Id == 0 means we are with a System::Object, make sure everything is set correctly...
        CROSSNET_ASSERT(info == NULL, "");
        CROSSNET_ASSERT(numInterfaceInfos == 0, "");
        CROSSNET_ASSERT(parentInterfaceMap == NULL, "");
        void * * interfaceMap = FindNextFreeSlots(MINIMUM_BASE_SLOT_SIZE);
        interfaceMap += (MINIMUM_BASE_SLOT_SIZE - OFFSET_FROM_END_OF_BASE_SLOT);
        interfaceMap[CURRENT_ID] = (void *)(id);
        interfaceMap[SIZE] = (void *)size;
        interfaceMap[NUMBER_OF_INTERFACES_AND_CLASSES] = (void *)(USED_SLOT);   // No classes, no interfaces, but still used
        interfaceMap[TYPEOF] = type;

        sNextFreeSlot = interfaceMap + 1;   // This is a special case...
                                            // This Id is zero, but we don't want this value to be picked up by the free slot
        UpdateFreeSlot();

        if (interfaceMap > sLastInterfaceMap)
        {
            sLastInterfaceMap = interfaceMap;
        }
        return (interfaceMap);
    }

    if (info == NULL)
    {
        CROSSNET_ASSERT(numInterfaceInfos == 0, "");

        if (parentInterfaceMap == NULL)
        {
            // Simplest case, no base interfaces, no parent implementation
            void * * interfaceMap = FindNextFreeSlots(MINIMUM_BASE_SLOT_SIZE);
            interfaceMap += (MINIMUM_BASE_SLOT_SIZE - OFFSET_FROM_END_OF_BASE_SLOT);
            interfaceMap[CURRENT_ID] = (void *)(id);
            interfaceMap[SIZE] = (void *)size;
            interfaceMap[NUMBER_OF_INTERFACES_AND_CLASSES] = (void *)(USED_SLOT);   // No classes, no interfaces
            interfaceMap[TYPEOF] = type;
            UpdateFreeSlot();

            if (interfaceMap > sLastInterfaceMap)
            {
                sLastInterfaceMap = interfaceMap;
            }
            return (interfaceMap);
        }

        // Otherwise just handle the standard default case...
    }
    else
    {
        CROSSNET_ASSERT(numInterfaceInfos != 0, "");
    }

    // There are several solutions but we do a simple brut force
    // It seems that if we were using bit mask we would be faster though... ;)

    // The current search is linear, in order to avoid wasting space...
    // We could speed up things a little bit if we were searching in random places
    // But it would waste more memory...

    int numBackFill = MINIMUM_BASE_SLOT_SIZE;   // Room for the ID and the number of interfaces and classes
    int numBaseClasses = 0;
    int * baseClasses = NULL;
    int baseClassId = 0;
    if (parentInterfaceMap != NULL)
    {
        // The caller provided an interface map, look at all the corresponding implementations
        int numInterfaces = GetNumInterfacesAndClasses(parentInterfaceMap, &numBaseClasses);
        numBaseClasses = GetNumClasses(parentInterfaceMap) + 1;         // Base class of the parent + the parent itself
        baseClasses = GetObjectList(parentInterfaceMap, numInterfaces);
        baseClassId = GetId(parentInterfaceMap);    // the parent Id is not in the list, we store it in baseClassId
        numBackFill += numBaseClasses;

        // We are not testing here, but one thing we should do is make sure that all the base class interfaces
        // Are overriden with the current set as well...

        // At the very least we compare the number...
        // TODO: Add more checks...
        CROSSNET_ASSERT(numInterfaceInfos >= numInterfaces, "");
    }
    numBackFill += numInterfaceInfos;

    void * * current = FindNextFreeSlots(numBackFill);

    for ( ; ; )
    {
        int i;
        for (i = 0 ; i < numInterfaceInfos ; ++i)
        {
            int interfaceId = info[i].mInterfaceId;
            // Offset the index to match the correct position
            interfaceId += (numBackFill - OFFSET_FROM_END_OF_BASE_SLOT);
            if (((current - sInterfaceMap) + interfaceId) >= sInterfaceMapSize)
            {
                CROSSNET_FAIL("InterfaceMap is not big enough!");
                return (NULL);
            }

            if (current[interfaceId] != NULL)
            {
                // This slot in the interface map is already full, this combination doesn't work
                break;
            }

            // This slot is empty, let's try the remaining interfaces to see if they fit
        }

        if (i == numInterfaceInfos)
        {
            // Enough room for all the interface wrappers!
            break;
        }

        // We could not map all the interfaces, let's go further then...
        // Skip one and look at the next free block of the size numBackFill slots
        current = FindNextFreeSlots(current + 1, numBackFill);
    }

    // Found the block we were interested in...
    current += (numBackFill - OFFSET_FROM_END_OF_BASE_SLOT);

    current[CURRENT_ID] = (void *)id;
    current[SIZE] = (void *)size;
    current[TYPEOF] = type;
    WriteNumInterfacesAndClasses(current, numInterfaceInfos, numBaseClasses);

    // Now let's write all the interface list and the object list
    int * interfaceList = GetInterfaceList(current);
    int * objectList = GetObjectList(current, numInterfaceInfos);

    for (int i = 0 ; i < numInterfaceInfos ; ++i)
    {
        *interfaceList-- = info[i].mInterfaceId;
    }

    if (numBaseClasses != 0)
    {
        *objectList-- = baseClassId;
        // Could use a memcpy here, I'm not sure it is that critical though...
        for (int i = 0 ; i < numBaseClasses - 1 ; ++i)
        {
            *objectList-- = *baseClasses--;
        }
    }

    // Finally let's write the interface wrappers
    for (int i = 0 ; i < numInterfaceInfos ; ++i)
    {
        int iid = info[i].mInterfaceId;
        CROSSNET_ASSERT(((current - sInterfaceMap) + iid) < sInterfaceMapSize, "Should not overflow here, as we tested earlier!");
        CROSSNET_ASSERT(current[iid] == NULL, "Should be empty as we tested earlier!");
        current[iid] = info[i].mInterfaceWrapper;
    }

    // Update the free slot if needed
    UpdateFreeSlot();

    if (current > sLastInterfaceMap)
    {
        sLastInterfaceMap = current;
    }

    // We are done here
    return (current);
}

void InterfaceMapper::UpdateFreeSlot()
{
    if (*sNextFreeSlot == NULL)
    {
        return;
    }
    bool foundFree = false;
    void * * nextFree = sNextFreeSlot;
    while ((nextFree - sInterfaceMap) < sInterfaceMapSize)
    {
        if (*nextFree == NULL)
        {
            // Found the next free slot
            foundFree = true;
            break;
        }
        ++nextFree;
    }
    if (foundFree == false)
    {
        // No more free slot in the interface table! It's completey packed with no fragmentation!
        // Next time, we won't be able to add an interface!
        CROSSNET_FAIL("InterfaceMap is full!");
        sNextFreeSlot = NULL;      // It will force the crash next time...
    }
    else
    {
        sNextFreeSlot = nextFree;
    }
}

void * * InterfaceMapper::FindNextFreeSlots(int numberOfSlots)
{
    return (FindNextFreeSlots(sNextFreeSlot, numberOfSlots));
}

void * * InterfaceMapper::FindNextFreeSlots(void * * currentSlot, int numberOfSlots)
{
    // Careful, this code doesn;t check if the 
    for ( ; ; )
    {
        void * * localSlot = currentSlot;
        bool collision = false;
        for (int i = 0 ; i < numberOfSlots ; ++i)
        {
            CROSSNET_ASSERT(localSlot < (sInterfaceMap + sInterfaceMapSize), "");
            if (localSlot[i] != NULL)
            {
                // There is a collision, instead of restarting at currentSlot + 1
                // We restart just after the collision...
                currentSlot += i + 1;
                collision = true;
                break;
            }
        }

        if (collision == false)
        {
            return (currentSlot);
        }
    }
}

int InterfaceMapper::RetrieveNextInterfaceId()
{
    int nextInterfaceId = sNextInterfaceId;

    // This loop has two purposes, first we find the next available id by comparing with the static ids
    // And we clean the list of static ids to optimize the search...
    for (;;)
    {
        bool collision = false;
        std::vector<int>::iterator itBegin = sStaticInterfaceId.begin();
        std::vector<int>::iterator itEnd = sStaticInterfaceId.end();
        std::vector<int>::iterator toDelete = itEnd;
        for (std::vector<int>::iterator it = itBegin ; it != itEnd ; ++it)
        {
            if (*it < nextInterfaceId)
            {
                toDelete = it;
                break;
            }
            if (*it == nextInterfaceId)
            {
                collision = true;
            }
        }

        if (toDelete != itEnd)
        {
            // There is one id to delete, erase it
            sStaticInterfaceId.erase(toDelete);
            // And continue the loop without looking at collision
            // as we want to erase all the obsolete static id
            // I believe there is a better to do this with STL but I don't have the time to look at it
            continue;
        }
        if (collision == false)
        {
            // Found one without collision, we are done...
            break;
        }
        // Some collision, let's go forward a bit...
        ++nextInterfaceId;
    }
    sNextInterfaceId = nextInterfaceId + 1;
    return (nextInterfaceId);
}

int InterfaceMapper::RetrieveNextObjectId()
{
    int nextObjectId = sNextObjectId;

    // This loop has two purposes, first we find the next available id by comparing with the static ids
    // And we clean the list of static ids to optimize the search...
    for (;;)
    {
        bool collision = false;
        std::vector<int>::iterator itBegin = sStaticObjectId.begin();
        std::vector<int>::iterator itEnd = sStaticObjectId.end();
        std::vector<int>::iterator toDelete = itEnd;
        for (std::vector<int>::iterator it = itBegin ; it != itEnd ; ++it)
        {
            if (*it > nextObjectId)
            {
                toDelete = it;
                break;
            }
            if (*it == nextObjectId)
            {
                collision = true;
            }
        }

        if (toDelete != itEnd)
        {
            // There is one id to delete, erase it
            sStaticObjectId.erase(toDelete);
            // And continue the loop without looking at collision
            // as we want to erase all the obsolete static id
            // I believe there is a better to do this with STL but I don't have the time to look at it
            continue;
        }
        if (collision == false)
        {
            // Found one without collision, we are done...
            break;
        }
        // Some collision, let's go backward a bit...
        --nextObjectId;
    }
    sNextObjectId = nextObjectId - 1;
    return (nextObjectId);
}

bool InterfaceMapper::InInterfaceMapSpace(void * pointer)
{
    if (pointer < sInterfaceMap)
    {
        // The pointer points before the start of the interface map buffer
        // It cannot be an interface map
        return (false);
    }
    if (pointer > sLastInterfaceMap)
    {
        // If that's after the last interface map, it is not in the space
        return (false);
    }
    return (true);
}

}
