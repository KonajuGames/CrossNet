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

#ifndef __CROSSNET_IINTERFACE_H__
#define __CROSSNET_IINTERFACE_H__

#include "CrossNetRuntime/InterfaceMapper.h"
#include "CrossNetRuntime/Internal/Primitives.h"
#include "CrossNetRuntime/System/Object.h"

namespace CrossNetRuntime
{
    class Object;

	// Guideline: All interfaces must be derived from IInterface
    class IInterface
    {
    public:
        virtual CROSSNET_FINLINE ~IInterface()
        {
            // Do nothing...
        }

        // Those methods are implemented to forward the call to System::Object
        // IInterface don't have those methods per say, but conceptually we can still call them
        // As any interface pointer can be actually pointed back to a System::Object, so everything works fine.
        // Also even when a struct implements an interface, the function is called directly on the struct
        // But as soon as it is boxed (and as such the interface pointer is manipulated), this code is being called
        // as it will call the boxed function, which in return will call the struct method
        CROSSNET_FINLINE
        System::String * ToString()
        {
            System::Object * temp = reinterpret_cast<System::Object *>(this);
            return (temp->ToString());
        }

        CROSSNET_FINLINE
        System::Boolean Equals(System::Object * obj)
        {
            System::Object * temp = reinterpret_cast<System::Object *>(this);
            return (temp->Equals(obj));
        }

        CROSSNET_FINLINE
        System::Int32 GetHashCode()
        {
            System::Object * temp = reinterpret_cast<System::Object *>(this);
            return (temp->GetHashCode());
        }

        // This m__InterfaceMap__ is not used for the interface
        // But it has to exist so it maps to the first member of System::Object
        // That way when we do an interface call, the interface map is accessed the same way
        // Remember that during a cast, the interface pointer is the same as the instance pointer

        // I'm wondering if we should derive Object from IInterface
        // This would make this code more robust...
        // The only issue is that it would increase the construction even more...
        // (Another level of VTable initialization...)
        void * *    m__InterfaceMap__;

        // For pure interface, this should be private (and declared but not implemented)
        // But because wrappers are deriving from interfaces, this should at least be protected
        // But actually because wrappers are actually created from other classes, this then needs to be public
        void * operator new(size_t size)
        {
            void * buffer = GetOptions().mUnmanagedAllocateCallback(size);
            return (buffer);
        }
        void operator delete(void * buffer)
        {
            GetOptions().mUnmanagedFreeCallback(buffer);
        }

    protected:
        CROSSNET_FINLINE
        IInterface()
        {
            // Do nothing...
        }

    private:
        void * operator new[](size_t size);
        void operator delete[](void * buffer);
    };

// In debug we add another layer for the interface call
// We make sure that the type can be casted to the correct type
// In release, this won't be needed...
#define INTERFACE__CALL(instance, interfaceName)   \
    (static_cast<interfaceName *>(::CrossNetRuntime::FastCast<interfaceName>(instance)->m__InterfaceMap__[interfaceName::__GetId__()]))

}

#endif
