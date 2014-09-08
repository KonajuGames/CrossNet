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

#ifndef __CROSSNET_BOX_H__
#define __CROSSNET_BOX_H__

#include "CrossNetRuntime/System/Object.h"
#include "CrossNetRuntime/Internal/Cast.h"

namespace CrossNetRuntime
{
    template <typename U, typename V>
    static CROSSNET_FINLINE
    U * Box(const V & instance)
    {
        System::Object * obj = BoxedObject<CrossNetRuntime::GenWrapper< V >::ConvertedType >::__Create__(instance);
        CROSSNET_FATAL(Cast<U>(obj) != NULL, STRINGIFY3("Could not cast the instance to the class ", U, "!"));
        return (U *)(obj);
    }

    template <typename U, typename V, typename W>
    static CROSSNET_FINLINE
    U * BoxEnum(const W & instance)
    {
        return (U *)(BoxedObject<CrossNetRuntime::GenWrapper< V >::ConvertedType >::__Create__(instance));
    }

    // We define this version for some types already boxed
    // This can actually happen with the generics when the types are not known ahead of time
    template <typename U, typename V>
    static CROSSNET_FINLINE
    U * Box(V * instance)
    {
        // We make sure that the instance can be cast correctly
        if (instance != NULL)
        {
            // Also with generics, the pointer can be NULL when we call CrossNetRuntime::Box
            // We don't want to assert because of that...
            CROSSNET_FATAL(Cast<U>(instance) != NULL, STRINGIFY3("Could not cast the instance to the class ", U, "!"));
        }
        // Then we do an unsafe cast (fastest in release)
        // as for any instance, interface or classes are pointing on the same pointer (regardless of the type)
        return (U *)(instance);
    }

    template <typename U>
    class BoxedObject : public System::Object
    {
    public:
        // This is a particular type, by itself it doesn't have any ID, it just forwards the boxed object ID

        static BoxedObject * __Create__(const U & value)
        {
            return (new BoxedObject(value));
        }

        CROSSNET_FINLINE
        operator U()
        {
            return (mBoxedObject);
        }

        CROSSNET_FINLINE
        U * GetUnboxedAddress()
        {
            return (&mBoxedObject);
        }

        virtual System::String *    ToString()
        {
            return (mBoxedObject.ToString());
        }

        virtual bool    Equals(System::Object * other)
        {
            return (mBoxedObject.Equals(other));
        }

        virtual System::Int32       GetHashCode()
        {
            return (mBoxedObject.GetHashCode());
        }

        U & get_Value()
        {
            return (mBoxedObject);
        }

    private:
        BoxedObject(const U & value)
            :   mBoxedObject(value)
        {
            m__InterfaceMap__ = U::__GetInterfaceMap__();

            // When boxing an object, make sure that the size is set correctly
            // We could do it during boxing but try to avoid the cost of writing to the interface map for each boxed object
            // We won't detect the issue at compile time, nor at boot, but only when boxing the type
            // Worst case scenario: If we detect the issue, later we could put the correct value back while still having the assert
            CROSSNET_ASSERT(CrossNetRuntime::InterfaceMapper::GetSize(m__InterfaceMap__) == sizeof(BoxedObject), "");
        }

        U   mBoxedObject;
    };

    template <typename U>
    class TypeParser
    {
    public:
        static U   Unbox(System::Object * instance)
        {
            CROSSNET_FATAL(instance->__Cast__(U::__GetId__()) != NULL, STRINGIFY3("Could not unbox the instance to the structure type ", U, "!"));
            BoxedObject<U> * boxedObject = static_cast<BoxedObject<U> * >(instance);
            return (*boxedObject);
        }
    };

    // I believe this is the only place in CrossNet that we are doing partial specialization
    // This is used to determine the real type when unboxing an object that is actually a reference object...
    // The main cases are for the arrays (when converting object item to the real case), or with generics...
    // Just cross the finger that your compiler is supporting this otherwise you'll have to deactivate some features...
    // There is probably other ways to do the same but it seems this one is the simplest...
    // Also you can see that the partial specialization is very simple, just one item,
    // so chances are that your compiler supports this without any issue...

    // Also we have to use an intermediate class as partial specialization doesn't work on templated functions
    template <typename U>
    class TypeParser<U *>
    {
    public:
        static U * Unbox(System::Object * instance)
        {
            if (instance != NULL)
            {
                CROSSNET_FATAL(instance->__Cast__(U::__GetId__()) != NULL,
                    STRINGIFY3("Could not unbox the instance to the class type ", U, "!"));
            }
            return (U *)(instance);
        }
    };

    template <typename U>
    CROSSNET_FINLINE
    U Unbox(System::Object * instance)
    {
        return (TypeParser<U>::Unbox(instance));
    }

    template <typename U>
    CROSSNET_FINLINE
    U Unbox(CrossNetRuntime::IInterface * instance)
    {
        return (TypeParser<U>::Unbox((System::Object *)instance));
    }
}

#endif
