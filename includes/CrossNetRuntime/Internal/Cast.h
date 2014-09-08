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

#ifndef __CROSSNET_CAST_H__
#define __CROSSNET_CAST_H__

#include "CrossNetRuntime/System/Object.h"

namespace CrossNetRuntime
{
    template <typename U, typename V>
    static CROSSNET_FINLINE
    U UnsafeCast(V other)
    {
        // A safer approach would be to use reinterpret_cast as it would almost cast anything even unrelated types
        // But in order to discover (and solve) tricky casting issue, we use static_cast

        // If your product doesn't compile here because of a cast not handled gracefully by CrossNet
        // Just change locally static_cast by reinterpret_cast

        // If one day, we find a case where static_cast doesn't work even with carefully specialized cast code
        // then we will switch definitively to reinterpret_cast.
        return static_cast<U>(other);
    }

    template <typename U, typename V>
    static CROSSNET_FINLINE
    U ReinterpretCast(V other)
    {
        return reinterpret_cast<U>(other);


    //  For the moment use C cast, instead of reinterpret_cast...
    //  The reason is that in one case (like for PointerWrapper) it seems we can't use reinterpret_cast
    //  Even if a user conversion has been provided...
    //    return (U)(other);
    }

    template <typename U, typename V>
    static CROSSNET_FINLINE
    U EnumCast(V other)
    {
        return static_cast<U>(other);
    }

    template <typename T>
    static CROSSNET_FINLINE
    T * Cast(System::Object * instance)
    {
        if (instance == NULL)
        {
            return (NULL);
        }
        void * pointer = instance->__Cast__(T::__GetId__());
        CROSSNET_FATAL(pointer != NULL, STRINGIFY3("Could not cast the instance to the type ", T, "!"));
        return (T *)(pointer);
    }

    template <typename T>
    static CROSSNET_FINLINE
    T * Cast(CrossNetRuntime::IInterface * instance)
    {
        if (instance == NULL)
        {
            return (NULL);
        }
        void * pointer = ((System::Object *)instance)->__Cast__(T::__GetId__());
        CROSSNET_FATAL(pointer != NULL, STRINGIFY3("Could not cast the instance to the type ", T, "!"));
        return (T *)(pointer);
    }

    // Those fast casts assume that the instance pointer and the cast pointer are actually the same address
    // Which is the case in CrossNet implementation (as well as .Net).
    template <typename T>
    static CROSSNET_FINLINE
    T * FastCast(System::Object * instance)
    {
        CROSSNET_FATAL((instance == NULL) || (instance->__Cast__(T::__GetId__()) != NULL), STRINGIFY3("Could not cast the instance to the type ", T, "!"));
        return (T *)(instance);
    }

    template <typename T>
    static CROSSNET_FINLINE
    T * FastCast(CrossNetRuntime::IInterface * instance)
    {
        CROSSNET_FATAL((instance == NULL) || (((System::Object *)instance)->__Cast__(T::__GetId__()) != NULL), STRINGIFY3("Could not cast the instance to the type ", T, "!"));
        return (T *)(instance);
    }

    // For the moment this is the same code as CrossNetRuntime::Cast
    // We might want to change that later...
    template <typename T>
    static CROSSNET_FINLINE
    T * InterfaceCast(System::Object * instance)
    {
        if (instance == NULL)
        {
            return (NULL);
        }
        void * pointer = instance->__Cast__(T::__GetId__());
        CROSSNET_FATAL(pointer != NULL, STRINGIFY3("Could not cast the instance to the interface ", T, "!"));
        return (T *)(pointer);
    }

    template <typename T>
    static CROSSNET_FINLINE
    T * InterfaceCast(CrossNetRuntime::IInterface * instance)
    {
        if (instance == NULL)
        {
            return (NULL);
        }
        void * pointer = ((System::Object *)instance)->__Cast__(T::__GetId__());
        CROSSNET_FATAL(pointer != NULL, STRINGIFY3("Could not cast the instance to the interface ", T, "!"));
        return (T *)(pointer);
    }

    template <typename T, typename U>
    static CROSSNET_FINLINE
    T * InterfaceCast(U instance)
    {
        return (Box<T>(instance));
    }

    // This interface cast is used for NULL pointers passed.
    // NULL is actually passed as the integer 0
    // This is actually hard-coded in the compiler that 0 can represent NULL pointer for any type!
    // We check in this function that the value passed corresponds to a NULL pointer.
    // Obviously nothing guarantees that the user did not pass 0 explicitly but this should help detect other issues.
    template <typename T>
    static CROSSNET_FINLINE
    T * InterfaceCast(int integer)
    {
        CROSSNET_FATAL(integer == NULL, "The value was not NULL!");
        return (T *)(NULL);
    }

    template <typename T>
    class BaseTypeWrapper;

    // This is interface cast is used for primitive types used as generic parameter
    // The goal is to differentiate with the version above (with int as parameter for NULL pointers).
    template <typename T, typename U>
    static CROSSNET_FINLINE
    T * InterfaceCast(typename CrossNetRuntime::BaseTypeWrapper<U> value)
    {
        return (Box<T>((U)value));
    }

    template <typename T>
    static CROSSNET_FINLINE
    T * AsCast(System::Object * instance)
    {
        if (instance == NULL)
        {
            return (NULL);
        }
        void * pointer = instance->__Cast__(T::__GetId__());
        return (T *)(pointer);
    }

    template <typename T>
    static CROSSNET_FINLINE
    T * AsCast(CrossNetRuntime::IInterface * instance)
    {
        if (instance == NULL)
        {
            return (NULL);
        }
        void * pointer = ((System::Object *)(instance))->__Cast__(T::__GetId__());
        return (T *)(pointer);
    }

    // "as" with a value type as right member instead of a reference
    //  This is useful for generic code for example
    template <typename T, typename V>
    static CROSSNET_FINLINE
    T * AsCast(const V & o)
    {
        // First box the object (so we can get the corresponding interface map)
        // TODO: Get the interface map without having to box the object!
        System::Object * instance = Box<::System::Object>(o);
        if (instance == NULL)
        {
            // In some case like with generics, the object passed will actually be a NULL pointer
            // So after the boxing, it will still return NULL
            // We don't want to crash during the cast.
            // In this case we know that we cannot do the conversion
            return (NULL);
        }
        void * pointer = instance->__Cast__(T::__GetId__());
        return (T *)(pointer);
    }

    template <typename T>
    static CROSSNET_FINLINE
    bool IsCast(System::Object * instance)
    {
        if (instance == NULL)
        {
            return (false);
        }
        void * pointer = instance->__Cast__(T::__GetId__());
        return (pointer != NULL);
    }

    template <typename T>
    static CROSSNET_FINLINE
    bool IsCast(CrossNetRuntime::IInterface * instance)
    {
        if (instance == NULL)
        {
            return (false);
        }
        void * pointer = (System::Object *)(instance)->__Cast__(T::__GetId__());
        return (pointer != NULL);
    }

    // "is" with a value type as right member instead of a reference
    //  This is useful for generic code for example
    template <typename T, typename V>
    static CROSSNET_FINLINE
    bool IsCast(const V & o)
    {
        // First box the object (so we can get the corresponding interface map)
        // TODO: Get the interface map without having to box the object!
        System::Object * instance = Box<::System::Object>(o);
        if (instance == NULL)
        {
            // In some case like with generics, the object passed will actually be a NULL pointer
            // So after the boxing, it will still return NULL
            // We don't want to crash during the cast.
            // In this case we know that we cannot do the conversion
            return (false);
        }
        void * pointer = instance->__Cast__(T::__GetId__());
        return (pointer != NULL);
    }

}

#endif
