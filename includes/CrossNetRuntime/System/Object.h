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

#ifndef __SYSTEM_OBJECT_H__
#define	__SYSTEM_OBJECT_H__

#include "CrossNetRuntime/Assert.h"
#include "CrossNetRuntime/Defines.h"
#include "CrossNetRuntime/InterfaceMapper.h"
#include "CrossNetRuntime/GC/GCAllocator.h"
#include "CrossNetRuntime/Internal/Primitives.h"
#include "CrossNetRuntime/Internal/NewDelete.h"

// For information: Most of the virtual methods here are forced inline
//      The reason is that if you are statically calling them (like GCObject::GetFlags(); )
//      The code will be inlined instead of doing a function call, it will result in a smaller / faster code in most cases

namespace CrossNetRuntime
{
    class GCManager;
}

namespace System
{
    // Guideline: All implementations must be derived from Object
    class Object
    {
	public:
        CN_STATIC_ID(0)         //  Object should be one of the rare class to have a static ID
                                //  By the way the ID = 0 is special here as by definition object ID < 0
                                //  In reality we don't want to compare with this IID, the parsing should take care of that

        // Has to exist in the same order as IInterface
        // As both classes actually point to the same object (in some extent)
        void * *    m__InterfaceMap__;

        static Object * __Create__()
        {
            Object * __temp__ = new Object();
            __temp__->m__InterfaceMap__ = __GetInterfaceMap__();
            // No ctor__ function to call here... 
            return (__temp__);
        }

        CROSSNET_FINLINE
        static System::Boolean      Equals(Object * a, Object * b)
        {
            if (a == NULL)
            {
                // If first is NULL, the second must be NULL as well
                return (b == NULL);
            }
            return (a->Equals(b));
        }

/*
 *  No luck - it doesn't seem to work...
 *
        // To simplify the parsing, define this method that is used as fallback position when comparing two object types
        // In some cases (like comparison between one string and one object), CrossNet might interpret it as comparison between 2 strings
        // We could solve the parsing bug (and call Equals(Object *, Object *) instead, but this actually safely solve the issue...
        // This doesn't have side effect as specific type would overide their own version and can't define an (Object *, Object *) anyway...
        CROSSNET_FINLINE
        static System::Boolean  op_Equality(Object * a, Object * b)
        {
            if (a == NULL)
            {
                // If first is NULL, the second must be NULL as well
                return (b == NULL);
            }
            return (a->Equals(b));
        }
*/
        CROSSNET_FINLINE
        static System::Boolean  ReferenceEquals(Object * a, Object * b)
        {
            // Pointer comparison...
            return (a == b);
        }
 
        CROSSNET_FINLINE
        virtual System::Type * GetType()
        {
            return (CrossNetRuntime::InterfaceMapper::GetType(m__InterfaceMap__));
        }

		// Queries a type from the instance
        //  It can be casting from another type, an interface...
        //      Will return NULL if the type can't be casted
        void * __Cast__(int iid);

        virtual System::String * ToString();

        virtual System::Boolean Equals(Object * other)
        {
            // Looking at results from unit-tests, it seems that default implementation compares just pointer
            // ReferenceEquals is the same but will return the same result even if Equals has been overriden
            return (this == other);
        }

        static
        System::Boolean InternalEquals(System::Object * left, System::Object * right)
        {
            // Function rarely used... Does it have to be different from Equals(a, b) ?
            return (left == right);
        }

        virtual System::Int32   GetHashCode()
        {
            // Currently use the pointer as hashcode, note that when defragmentation will be used
            // This won't work anymore...
            return (System::Int32)(this);
        }

        static
        System::Int32 InternalGetHashCode(System::Object * obj)
        {
            if (obj == NULL)
            {
                return (13);    // Returns 13 if the pointer is not set...
            }
            // Currently use the pointer as hashcode, note that when defragmentation will be used
            // This won't work anymore...
            return (System::Int32)(obj);
        }

        template <typename T>
        CROSSNET_FINLINE
        static T * __Lock__(T * obj)
        {
            obj->__Lock__(0);    // No thread id yet...
            return (obj);
        }

        template <typename T>
        CROSSNET_FINLINE
        static T * __Unlock__(T * obj)
        {
            obj->__Unlock__(0);  // No thread id yet...
            return (obj);
        }

        System::Object *    MemberwiseClone()
        {
            size_t size;

            if ((m__AllFlags__ & __DYN_ALLOC__) != 0)
            {
                // Dynamic size, like strings or arrays
                size = (size_t)__GetVariableSize__();
            }
            else
            {
                // Standard static size, get the size from the interface map
                size = CrossNetRuntime::InterfaceMapper::GetSize(m__InterfaceMap__);
            }
            // Create an object with the same size
            void * newObject = System::Object::operator new(size);
            // Copy byte by byte all the members
            __memcopy__(newObject, this, size);
            // Done, we can return...
            return static_cast<System::Object *>(newObject);
        }

	protected:
        CROSSNET_FINLINE
		Object()
            :
            m__AllFlags__(__MARKER_AT_CREATION__)
#if DEBUG
            ,m__InterfaceMap__((void * *)__FAKE_INTERFACE_MAP__)
#endif
        {
			// Do nothing...
		}

        CROSSNET_FINLINE
		Object(unsigned int flags)
            :
            m__AllFlags__(__MARKER_AT_CREATION__ | flags)
#if DEBUG
            ,m__InterfaceMap__((void * *)__FAKE_INTERFACE_MAP__)
#endif
        {
			// Do nothing...
		}

        CROSSNET_FINLINE
		virtual ~Object()
		{
            // Each object is collected anyway, so at least we'll detect incorrectly set interface map
            //  During the destruction of the object
            CROSSNET_ASSERT(m__InterfaceMap__ != (void * * )__FAKE_INTERFACE_MAP__, "Interface Map not initialized correctly!");

			// We should call the destructor only from within GCManager
			// If this is not the case, then it is wrong and it is a bug in the caller code
	#ifdef	DESTRUCT_GCOBJECT_CALLBACK
            // We can register the check here just to make sure...
            ::CrossNetRuntime::OnDestructObjectPtr ptr = ::CrossNetRuntime::GetOptions().mDestructGCObjectCallback;
            if (ptr != NULL)
            {
                ptr(this);
            }
	#endif
		}

        virtual int __GetVariableSize__()
        {
            // The default implementation returns a negative number to detect incorrect behavior
            //  The reason is that for standard case, we have to use the interface map
            //  The only moment where this method should be called is for arrays and strings
            //  But they are allocating memory differently...
            //  They should be the only one to override that particular function, and return the corresponding size
            CROSSNET_FAIL("");
            return (-1);
        }

        // Define the default so we can add a couple of checks
        CROSSNET_FINLINE
        void __ctor__()
        {
            // Do nothing...
            CROSSNET_ASSERT(m__InterfaceMap__ != (void * * )__FAKE_INTERFACE_MAP__, "Interface Map not initialized correctly!");
        }

        CROSSNET_FINLINE
        void __Finalize__()
        {
            // Do nothing for the moment...
            // TODO:    See what implementation is needed here...
        }

		// Gets the mark of the GC object
        CROSSNET_FINLINE
		unsigned int __GetMark__() const
        {
            return (m__AllFlags__ & 0xff);
        }

		// Gets called when a GC object needs to be collected
		// A standard / default implementation is "delete this;"
        CROSSNET_FINLINE
        void __OnCollect__()
        {
            // Call the destructor without calling the deallocation
            // The reason is that the deallocation will be done by the GC
            // The GC is doing the deallocation as it has already the size information
            // during the parsing, so we don't have to calculate it two times...
            this->~Object();
        }

        // Gets the flags of the instance
        CROSSNET_FINLINE
        unsigned int    __GetFlags__() const
        {
            return (m__AllFlags__);
        }

        // Masks and sets the flags of the instance
        CROSSNET_FINLINE
        void            __SetFlags__(unsigned int mask, unsigned int set)
        {
            m__AllFlags__ &= ~mask;
            m__AllFlags__ |= set;
        }

        CROSSNET_FINLINE
        void            __SetFixed__(bool fixed)
        {
            if (fixed)
            {
                __SetFlags__(__FIXED__, __FIXED__);
            }
            else
            {
                __SetFlags__(__FIXED__, 0);
            }
        }

		// Traces the GC Object, if the mark is different, set the mark to the current mark
		// And trace all the pointed pointers
		virtual void			__Trace__(unsigned char /* currentMark */)
        {
            // Do nothing as System.Object doesn't point to any other class
        }

		// Locks the GC object - used for multi-threading application
		// Another thread calling Lock() as well will wait until this object gets unlocked
		// Lock() needs to be re-entrant (threadId is used to detech the thread currently carrying the lock).
		void			__Lock__(unsigned short /*threadId*/)
        {
        }

		// Unlocks the GC object.
		void			__Unlock__(unsigned short /*threadId*/)
        {
        }

        enum Flags
        {
            __COLLECTED__   =   (1 << 8),
            __FIXED__       =   (1 << 9),
            __ARRAY__       =   (1 << 10),      //  We need to markup the array in a special manner for GC
            __STRING__      =   (1 << 11),      //  Same for the strings

            __DYN_ALLOC__   =   __ARRAY__ | __STRING__,
        };

        // Standard new / delete operators are protected so derived class can use it
        void * operator new(size_t size)
        {
            void * buffer = ::CrossNetRuntime::GCAllocator::Allocate(size);
            // We clear everything after System::Object instance
            __memclear__((unsigned char *)(buffer) + sizeof(System::Object), size - sizeof(System::Object));
            return (buffer);
        }

        // We should declare but not define this function
        //  But it seems we will have link errors
        void operator delete(void * /*buffer*/)
        {
            CROSSNET_FAIL("Should not call delete but the destructor...");
        }

    private:
        // Private and declare but not defined as we should never use this...
        void * operator new[](size_t size);
        void operator delete[](void * buffer);
		// Declared but not implemented on purpose
		Object(const Object & other);
		Object& operator=(const Object & other);

        // Lowest 8 bits current mark
        // Higher 8 bits flags
        // Highest 16 bits - thread Id
        unsigned int m__AllFlags__;

        static const int            __FAKE_INTERFACE_MAP__  = 0x31415927;
        static const unsigned char  __MARKER_AT_CREATION__  = 0;

		// GCManager is friend so it can call the protected destructor and private members
        friend class ::CrossNetRuntime::GCManager;
    };
}

#endif


