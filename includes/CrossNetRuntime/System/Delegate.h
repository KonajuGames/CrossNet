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

#ifndef __SYSTEM_DELEGATE_H__
#define __SYSTEM_DELEGATE_H__

#include "CrossNetRuntime/System/Object.h"

namespace System
{
    namespace Reflection
    {
        class MethodInfo;
    }

    template <typename T>
    class Array__G;

    class Delegate : public System::Object
    {
    public:
        CN_DYNAMIC_ID()

        static System::Delegate * Combine(System::Delegate * a, System::Delegate * b);
        static System::Delegate * Combine(::System::Array__G< ::System::Delegate * > *);
        static System::Delegate * Remove(System::Delegate * source, System::Delegate * value);

        static System::Delegate * InternalCreateDelegate(System::Type * type, System::Object * firstArgument, System::Reflection::MethodInfo * method);

        System::Array__G<System::Delegate *> * GetInvocationList();
        System::Reflection::MethodInfo * get_Method();
        System::Object * get_Target();
        System::Object * Clone();

        System::Object * DynamicInvoke(::System::Array__G< ::System::Object * > *);

    protected:
        virtual System::Delegate * CombineImpl(System::Delegate * other);
        virtual System::Delegate * RemoveImpl(System::Delegate * other);

        // An interesting C++ feature - it seems that MulticastDelegate that derives publicly from Delegate
        // Can't access protected methods of Delegate
        // Make it friend for the moment
        // TODO: See why friend is needed!
        friend class MulticastDelegate;
    };

// It might be possible to have only one functor defined (for non-generic and generic).
// The trick would be to change the className to be actually an already defined type.
// Like we would do first:
//      struct className;
// or:  template <typename T> struct className;
// And the struct would just be passed as template parameter in the functor class.
// This way, we would not have to care if the delegate was or not a generic
// We would then have only one macro instead of two... And would remove some of the parsing...
// We can revisit this later...

// Another thing, related to performance this time:
// Currently we only have one functor implementation able to manage static call, method call and interface call
// Not only the memory consumption is a bit higher, but every invoke we have to do between one to two tests
// We could actually create three different implementations optimized for each case.
// The code would be as long but more optimized / specialized (without union and if tests).
// A little downside would mean that we would have to add a bit more parsing during the delegate creation
// to select the correct implementation (it's really not a big issue though).
// Although it "might" create a bigger issue with generics...
// I'll have to think about it more and come up with some unit-tests to check the behavior...

// About the Interface ID, I don't believe that the specific implementation has to be with a different ID
//  I suppose that only the delegate class itself should...
//  Note: Actually this is currently needed but it should not be the case after the optimizations are done.

// Optimization:
//  Move the various members up into delegate. Just cast in the impl as needed.
//  This will make Equals() really straightforward.
//  We will be able to remove also the ID definition in the impl.
//  Without even thinking about it more than 30 seconds, I believe there is much more to do...
//  Remember to remove the InterfaceMap initialization in the derived class as well, this will be redundant...

#define CREATE_DELEGATE(className, returnKeyword, returnValue, methodSignature, parameters) \
    class className : public ::System::MulticastDelegate                    \
    {                                                                       \
    public:                                                                 \
        CN_MULTIPLE_DYNAMIC_OBJECT_ID0(sizeof(className), ::System::MulticastDelegate::__GetInterfaceMap__())     \
        className()                                                         \
        {                                                                   \
            m__InterfaceMap__ = __GetInterfaceMap__();                          \
        }                                                                   \
        virtual returnValue Invoke(methodSignature) = 0;                    \
                                                                            \
        enum Mode                                                           \
        {                                                                   \
            CALL_STATIC_METHOD,                                             \
            CALL_METHOD,                                                    \
            CALL_INTERFACE_METHOD,                                          \
        };                                                                  \
    };                                                                      \
                                                                            \
    template <typename __T__>                                               \
    class className##__FUNCTOR__ : public className                         \
    {                                                                       \
    private:                                                                \
        CN_MULTIPLE_DYNAMIC_OBJECT_ID0(sizeof(className##__FUNCTOR__), className::__GetInterfaceMap__())    \
        typedef returnValue (__T__::*MethodSignature)(methodSignature);     \
        typedef returnValue (__T__::*InterfaceCallSignature)(void * __instance__ __COMMA##methodSignature); \
        typedef returnValue (*MethodSignatureStatic)(methodSignature);      \
                                                                            \
        Mode    mMode;                                                      \
        __T__ * mInstance;                                                  \
        union                                                               \
        {                                                                   \
            MethodSignature         mMethod;                                \
            InterfaceCallSignature  mInterfaceCall;                         \
            MethodSignatureStatic   mMethodStatic;                          \
        };                                                                  \
    public:                                                                 \
        className##__FUNCTOR__(__T__ * instance, MethodSignature method)    \
            :   mMode(CALL_METHOD), mInstance(instance), mMethod(method)    \
        {   CROSSNET_FATAL(mInstance != NULL, "Instance should be set!");   \
            m__InterfaceMap__ = __GetInterfaceMap__();  }                       \
        className##__FUNCTOR__(__T__ * instance, InterfaceCallSignature method) \
            :   mMode(CALL_INTERFACE_METHOD), mInstance(instance), mInterfaceCall(method)  \
        {   CROSSNET_FATAL(mInstance != NULL, "Instance should be set!");   \
            m__InterfaceMap__ = __GetInterfaceMap__();  }                       \
        className##__FUNCTOR__(MethodSignatureStatic method)                \
            :   mMode(CALL_STATIC_METHOD), mInstance(NULL), mMethodStatic(method)   \
        {   m__InterfaceMap__ = __GetInterfaceMap__();  }                       \
                                                                            \
        virtual returnValue Invoke(methodSignature)                         \
        {                                                                   \
            /* Do the multicast delegate first... */                        \
            /* So there is one return value set correctly */                \
            if (mDelegates.empty() == false)                                \
            {                                                               \
                std::vector<::System::Delegate *>::iterator it, itEnd;      \
                it = mDelegates.begin();                                    \
                itEnd = mDelegates.end();                                   \
                for ( ; it != itEnd ; ++it)                                 \
                {                                                           \
                    ((className *)(*it))->Invoke(parameters);               \
                }                                                           \
            }                                                               \
            if (mInstance != NULL)                                          \
            {                                                               \
                if (mMode == CALL_INTERFACE_METHOD)                         \
                {                                                           \
                    returnKeyword ((INTERFACE__CALL(mInstance, __T__)->*mInterfaceCall)(mInstance __COMMA##parameters)); \
                }                                                           \
                else                                                        \
                {                                                           \
                    CROSSNET_FATAL(mMode == CALL_METHOD, "Not initialized correctly!"); \
                    returnKeyword (*mInstance.*mMethod)(parameters);        \
                }                                                           \
            }                                                               \
            else                                                            \
            {                                                               \
                CROSSNET_FATAL(mMode == CALL_STATIC_METHOD, "The instance was NULL when trying to do a method or interface call!"); \
                returnKeyword (mMethodStatic(parameters));                  \
            }                                                               \
        }                                                                   \
        virtual ::System::Boolean Equals(::System::Object * obj)            \
        {                                                                   \
            className##__FUNCTOR__ * ptr = ::CrossNetRuntime::AsCast<className##__FUNCTOR__>(obj);   \
            if (ptr == NULL)                                                \
            {                                                               \
                return (false);                                             \
            }                                                               \
            if (mInstance != ptr->mInstance)                                \
            {                                                               \
                return (false);                                             \
            }                                                               \
            if (mMethod != ptr->mMethod)                                    \
            {                                                               \
                return (false);                                             \
            }                                                               \
            CROSSNET_ASSERT(mMode == ptr->mMode, "");                       \
            return (true);                                                  \
        }                                                                   \
    };

#define CREATE_DELEGATE__G(className, returnKeyword, returnValue, methodSignature, parameters, templateParametersDeclaration, templateParameters)   \
    template <templateParametersDeclaration>                                \
    class className : public ::System::MulticastDelegate                    \
    {                                                                       \
    public:                                                                 \
        CN_MULTIPLE_DYNAMIC_OBJECT_ID0(sizeof(className), ::System::MulticastDelegate::__GetInterfaceMap__())     \
        className()                                                         \
        {                                                                   \
            m__InterfaceMap__ = __GetInterfaceMap__();                          \
        }                                                                   \
        virtual returnValue Invoke(methodSignature) = 0;                    \
                                                                            \
        enum Mode                                                           \
        {                                                                   \
            CALL_STATIC_METHOD,                                             \
            CALL_METHOD,                                                    \
            CALL_INTERFACE_METHOD,                                          \
        };                                                                  \
    };                                                                      \
                                                                            \
    /* Put the template parameters after __T__, so user defaults are preserved */   \
    template <typename __T__, templateParametersDeclaration>                \
    class className##__FUNCTOR__ : public className<templateParameters>     \
    {                                                                       \
    private:                                                                \
        CN_MULTIPLE_DYNAMIC_OBJECT_ID0(sizeof(className##__FUNCTOR__), className<templateParameters>::__GetInterfaceMap__())    \
        typedef returnValue (__T__::*MethodSignature)(methodSignature);     \
        typedef returnValue (__T__::*InterfaceCallSignature)(void * __instance__ __COMMA##methodSignature); \
        typedef returnValue (*MethodSignatureStatic)(methodSignature);      \
                                                                            \
        Mode    mMode;                                                      \
        __T__ * mInstance;                                                  \
        union                                                               \
        {                                                                   \
            MethodSignature         mMethod;                                \
            InterfaceCallSignature  mInterfaceCall;                         \
            MethodSignatureStatic   mMethodStatic;                          \
        };                                                                  \
    public:                                                                 \
        className##__FUNCTOR__(__T__ * instance, MethodSignature method)    \
            :   mMode(CALL_METHOD), mInstance(instance), mMethod(method)    \
        {   CROSSNET_FATAL(mInstance != NULL, "Instance should be set!");   \
            m__InterfaceMap__ = __GetInterfaceMap__();  }                       \
        className##__FUNCTOR__(__T__ * instance, InterfaceCallSignature method) \
            :   mMode(CALL_INTERFACE_METHOD), mInstance(instance), mInterfaceCall(method)  \
        {   CROSSNET_FATAL(mInstance != NULL, "Instance should be set!");   \
            m__InterfaceMap__ = __GetInterfaceMap__();  }                       \
        className##__FUNCTOR__(MethodSignatureStatic method)                \
            :   mMode(CALL_STATIC_METHOD), mInstance(NULL), mMethodStatic(method)   \
        {   m__InterfaceMap__ = __GetInterfaceMap__();  }                       \
                                                                            \
        virtual returnValue Invoke(methodSignature)                         \
        {                                                                   \
            /* Do the multicast delegate first... */                        \
            /* So there is one return value set correctly */                \
            if (mDelegates.empty() == false)                                \
            {                                                               \
                std::vector<::System::Delegate *>::iterator it, itEnd;      \
                it = mDelegates.begin();                                    \
                itEnd = mDelegates.end();                                   \
                for ( ; it != itEnd ; ++it)                                 \
                {                                                           \
                    ((className *)(*it))->Invoke(parameters);               \
                }                                                           \
            }                                                               \
            if (mInstance != NULL)                                          \
            {                                                               \
                if (mMode == CALL_INTERFACE_METHOD)                         \
                {                                                           \
                    returnKeyword ((INTERFACE__CALL(mInstance, __T__)->*mInterfaceCall)(mInstance __COMMA##parameters));   \
                }                                                           \
                else                                                        \
                {                                                           \
                    CROSSNET_FATAL(mMode == CALL_METHOD, "Not initialized correctly!"); \
                    returnKeyword (*mInstance.*mMethod)(parameters);        \
                }                                                           \
            }                                                               \
            else                                                            \
            {                                                               \
                CROSSNET_FATAL(mMode == CALL_STATIC_METHOD, "The instance was NULL when trying to do a method or interface call!"); \
                returnKeyword (mMethodStatic(parameters));                  \
            }                                                               \
        }                                                                   \
        virtual ::System::Boolean Equals(::System::Object * obj)            \
        {                                                                   \
            className##__FUNCTOR__ * ptr = ::CrossNetRuntime::AsCast<className##__FUNCTOR__>(obj);       \
            if (ptr == NULL)                                                \
            {                                                               \
                return (false);                                             \
            }                                                               \
            if (mInstance != ptr->mInstance)                                \
            {                                                               \
                return (false);                                             \
            }                                                               \
            if (mMethod != ptr->mMethod)                                    \
            {                                                               \
                return (false);                                             \
            }                                                               \
            CROSSNET_ASSERT(mMode == ptr->mMode, "");                       \
            return (true);                                                  \
        }                                                                   \
    };
}

#endif