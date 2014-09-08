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

#ifndef __INTERFACE_MAP_H__
#define __INTERFACE_MAP_H__

#include <vector>
#include "CrossNetRuntime/Defines.h"
#include "CrossNetRuntime/InitOptions.h"

namespace System
{
    class Type;
}

namespace CrossNetRuntime
{
    class IInterface;

    struct InterfaceInfo
    {
    public:
        int             mInterfaceId;
        IInterface *    mInterfaceWrapper;
    };

    class InterfaceMapper
    {
    public:
        static void Setup(const ::CrossNetRuntime::InitOptions & options);
        static void Teardown();

        static void Trace(unsigned char currentMark);

        static void * * RegisterInterfaceStaticId(int staticId, InterfaceInfo * info = NULL, int numInterfaceInfos = 0);
        static void * * RegisterObjectStaticId(int staticId, size_t size, InterfaceInfo * info = NULL, int numInterfaceInfos = 0, void * * parentInterfaceMap = NULL);

        static void * * RegisterInterface(InterfaceInfo * info = NULL, int numInterfaceInfos = 0);
        static void * * RegisterObject(size_t size, InterfaceInfo * info = NULL, int numInterfaceInfos = 0, void * * parentInterfaceMap = NULL);

        CROSSNET_FINLINE
        static size_t   GetSize(void * * interfaceMap)
        {
            return (size_t)(interfaceMap[SIZE]);
        }

        CROSSNET_FINLINE
        static int      GetNumInterfaces(void * * interfaceMap)
        {
            int numInterfacesAndClasses = (int)(interfaceMap[NUMBER_OF_INTERFACES_AND_CLASSES]);
            int numInterfaces = numInterfacesAndClasses & USED_SLOT_MASK;   // Number of interfaces is the lower part...
            return (numInterfaces);
        }

        CROSSNET_FINLINE
        static int      GetNumClasses(void * * interfaceMap)
        {
            int numInterfacesAndClasses = (int)(interfaceMap[NUMBER_OF_INTERFACES_AND_CLASSES]);
            int numClasses = numInterfacesAndClasses >> 16;         // Number of classes is the higher part
            return (numClasses);
        }

        CROSSNET_FINLINE
        static int      GetNumInterfacesAndClasses(void * * interfaceMap, int * numClasses)
        {
            int numInterfacesAndClasses = (int)(interfaceMap[NUMBER_OF_INTERFACES_AND_CLASSES]);
            int numInterfaces = numInterfacesAndClasses & USED_SLOT_MASK;   // Number of interfaces is the lower part...
            *numClasses = numInterfacesAndClasses >> 16;                    // Number of classes is the higher part
            return (numInterfaces);
        }

        CROSSNET_FINLINE
        static void     WriteNumInterfacesAndClasses(void * * interfaceMap, int numInterfaces, int numClasses)
        {
            numInterfaces &= USED_SLOT_MASK;
            numClasses &= 0xffff;
            interfaceMap[NUMBER_OF_INTERFACES_AND_CLASSES] = (void *)((numClasses << 16) | numInterfaces | USED_SLOT);
        }

        // Interfaces are the closest to the interface map pointer, objects are further
        CROSSNET_FINLINE
        static int *    GetInterfaceList(void * * interfaceMap)
        {
            return (int *)(interfaceMap + LIST_OF_INTERFACES_AND_CLASSES);
        }

        // Skip the interfaces
        CROSSNET_FINLINE
        static int *    GetObjectList(void * * interfaceMap, int numInterfaces)
        {
            return (int * )(interfaceMap + LIST_OF_INTERFACES_AND_CLASSES - numInterfaces);
        }

        CROSSNET_FINLINE
        static int      GetId(void * * interfaceMap)
        {
            return (int)(*interfaceMap);
        }

        CROSSNET_FINLINE
        static System::Type * GetType(void * * interfaceMap)
        {
            return (System::Type *)(interfaceMap[TYPEOF]);
        }

        static bool InInterfaceMapSpace(void * pointer);

    private:
        InterfaceMapper();
        InterfaceMapper(const InterfaceMapper & other);
        ~InterfaceMapper();
        InterfaceMapper & operator =(const InterfaceMapper & other);

        static const int    MINIMUM_BASE_SLOT_SIZE = 4;
        static const int    OFFSET_FROM_END_OF_BASE_SLOT = 1;

        static const int    CURRENT_ID = 0;
        static const int    SIZE = -1;
        static const int    NUMBER_OF_INTERFACES_AND_CLASSES = -2;
        static const int    TYPEOF = -3;
        static const int    LIST_OF_INTERFACES_AND_CLASSES = -4;

        static const int    USED_SLOT = 0x8000;
        static const int    USED_SLOT_MASK = 0x7fff;

        static void * * CreateInterfaceMap(System::Type * type, int id, size_t size, InterfaceInfo * info, int numInterfaceInfos, void * * parentInterfaceMap);

        static void     UpdateFreeSlot();
        static void * * FindNextFreeSlots(int numberOfSlots);
        static void * * FindNextFreeSlots(void * * currentSlot, int numberOfSlots);
        static int      RetrieveNextInterfaceId();
        static int      RetrieveNextObjectId();

        static System::Type *   CreateSystemType();
        static void             TraceSystemType(System::Type * type, unsigned char currentMark);

        static void * *         sInterfaceMap;
        static int              sInterfaceMapSize;
        static void * *         sNextFreeSlot;
        static int              sNextInterfaceId;
        static int              sNextObjectId;
        static void *           sLastInterfaceMap;
        static std::vector<int> sStaticInterfaceId;
        static std::vector<int> sStaticObjectId;
        static std::vector<::System::Type *> sAllTypes;
    };
}


// Use this IID declaration if you know ahead of time your IID for the interface and you want to "reserve" it.
// This definition gives you the most speed and the less memory consumption at the cost of having less dynamic types.
// It's the user's responsability to make sure its IDs are not colliding (the interface mapper will check it at runtime).
// You can have a major issue if another assembly is colliding with your IID as you will have to regenerate them.
// If you are going to use them, it's better to use them on low level assmeblies that don't change a lot
// instead of several high-level assemblies that you are iterating over often.

// Also all the static IID interfaces / objects must be registered first to the InterfaceMapper before starting the registration
// of the dynamic IID. Failing to do so might create a collision (for example, a dynamic IID is allocated to 100,
// and then a static IID tries to be registered to 100 as well).

#define CN_STATIC_ID(iid)                                   \
    public:                                                 \
    static void * * s__InterfaceMap__;                      \
    static const int __Id__ = iid;                          \
    static void * * __GetInterfaceMap__()                   \
    {                                                       \
        return (s__InterfaceMap__);                         \
    }                                                       \
    public:                                                 \
    static void __RegisterId__();                           \
    static int __GetId__()                                  \
    {                                                       \
        return (__Id__);                                    \
    }

// Use this IID declaration for general purpose.
// This definition is decently fast (only one pointer dereferening to get the IID), and you don't have to care
// of the IID collision.
// Most of the interfaces / objects should use this.

// Compared to the two other implementations 

#define CN_DYNAMIC_ID()                                     \
    public:                                                 \
    static void * * s__InterfaceMap__;                      \
    static void * * __GetInterfaceMap__()                   \
    {                                                       \
        return (s__InterfaceMap__);                         \
    }                                                       \
    public:                                                 \
    static void __RegisterId__();                           \
    static int __GetId__()                                  \
    {                                                       \
        return reinterpret_cast<int>(*s__InterfaceMap__);   \
    }

#define CN_DYNAMIC_OBJECT_ID0(T)                            \
    public:                                                 \
    static void * * s__InterfaceMap__;                      \
    static void * * __GetInterfaceMap__()                   \
    {                                                       \
        return (s__InterfaceMap__);                         \
    }                                                       \
    public:                                                 \
    static void __RegisterId__()                            \
    {                                                       \
        void * * interfaceMap = CrossNetRuntime::InterfaceMapper::RegisterObject(T); \
        s__InterfaceMap__ = interfaceMap;                   \
    }                                                       \
    static int __GetId__()                                  \
    {                                                       \
        return reinterpret_cast<int>(*s__InterfaceMap__);   \
    }

#define CN_DYNAMIC_INTERFACE_ID0()                          \
    public:                                                 \
    static void * * s__InterfaceMap__;                      \
    static void * * __GetInterfaceMap__()                   \
    {                                                       \
        return (s__InterfaceMap__);                         \
    }                                                       \
    public:                                                 \
    static void __RegisterId__()                            \
    {                                                       \
        void * * interfaceMap = CrossNetRuntime::InterfaceMapper::RegisterInterface(); \
        s__InterfaceMap__ = interfaceMap;                   \
    }                                                       \
    static int __GetId__()                                  \
    {                                                       \
        return reinterpret_cast<int>(*s__InterfaceMap__);   \
    }

// Use this IID declaration only for templated / generic interfaces and objects.
// Although you could use this definition instead of the simpler dynamic IID, you really don't want to.
// This version is significantly slower than the two other versions, but it has the advantage of handling gracefully
// several specializations of the same code for template.
// So for example IMyInterfac<int> and IMyInterface<bool> will have different IIDs.
// For templated code, if you are using the dynamic IID instead of this version, you'll have link error for each
// specialization (for example IMyInterface<int>::s__InterfaceMap__ and IMyInterface<bool>::s__InterfaceMap__
// will be missing.

#define CN_MULTIPLE_DYNAMIC_INTERFACE_ID0()                 \
    public:                                                 \
    static void * * __GetInterfaceMap__()                   \
    {                                                       \
        static void * * s__InterfaceMap__ = NULL;           \
        if (s__InterfaceMap__ == NULL)                      \
        {                                                   \
            void * * interfaceMap = CrossNetRuntime::InterfaceMapper::RegisterInterface(); \
            s__InterfaceMap__ = interfaceMap;               \
        }                                                   \
        return (s__InterfaceMap__);                         \
    }                                                       \
    public:                                                 \
    static int __GetId__()                                  \
    {                                                       \
        void * * interfaceMap = __GetInterfaceMap__();      \
        return (int)(*interfaceMap);                        \
    }

// The reason we are using this is only for the reflection information...
// So we can tell on a given interface what interfaces are derived from
// I guess we could also use this to make sure that an implementation is
// implementing all the interfaces correctly...
#define CN_MULTIPLE_DYNAMIC_INTERFACE_ID(a)                 \
    public:                                                 \
    static void * * __GetInterfaceMap__()                   \
    {                                                       \
        static void * * s__InterfaceMap__ = NULL;           \
        if (s__InterfaceMap__ == NULL)                      \
        {                                                   \
            CrossNetRuntime::InterfaceInfo info[] =            \
            {   a   };                                      \
            void * * interfaceMap = CrossNetRuntime::InterfaceMapper::RegisterInterface(info, sizeof(info) / sizeof(info[0])); \
            s__InterfaceMap__ = interfaceMap;               \
        }                                                   \
        return (s__InterfaceMap__);                         \
    }                                                       \
    public:                                                 \
    static int __GetId__()                                  \
    {                                                       \
        void * * interfaceMap = __GetInterfaceMap__();      \
        return (int)(*interfaceMap);                        \
    }


#define CN_MULTIPLE_DYNAMIC_OBJECT_ID0(T, b)                \
    public:                                                 \
    static void * * __GetInterfaceMap__()                   \
    {                                                       \
        static void * * s__InterfaceMap__ = NULL;           \
        if (s__InterfaceMap__ == NULL)                      \
        {                                                   \
            void * * interfaceMap = CrossNetRuntime::InterfaceMapper::RegisterObject(T, NULL, 0, b);  \
            s__InterfaceMap__ = interfaceMap;               \
        }                                                   \
        return (s__InterfaceMap__);                         \
    }                                                       \
    public:                                                 \
    static int __GetId__()                                  \
    {                                                       \
        void * * interfaceMap = __GetInterfaceMap__();      \
        return (int)(*interfaceMap);                        \
    }

#define CN_MULTIPLE_DYNAMIC_OBJECT_ID(T, a, b)              \
    public:                                                 \
    static void * * __GetInterfaceMap__()                   \
    {                                                       \
        static void * * s__InterfaceMap__ = NULL;           \
        if (s__InterfaceMap__ == NULL)                      \
        {                                                   \
            CrossNetRuntime::InterfaceInfo info[] =            \
            {   a   };                                      \
            void * * interfaceMap = CrossNetRuntime::InterfaceMapper::RegisterObject(T, info, sizeof(info) / sizeof(info[0]), b); \
            s__InterfaceMap__ = interfaceMap;               \
        }                                                   \
        return (s__InterfaceMap__);                         \
    }                                                       \
    public:                                                 \
    static int __GetId__()                                  \
    {                                                       \
        void * * interfaceMap = __GetInterfaceMap__();      \
        return (int)(*interfaceMap);                        \
    }

#define CN_IMPLEMENT(a) {   a::__GetId__(), new a   }

// See the commen on CN_MULTIPLE_DYNAMIC_INTERFACE_ID
#define CN_INTERFACE(a) {   a::__GetId__(), NULL    }

#endif

