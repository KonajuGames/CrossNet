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

#ifndef __GCMANAGER_H__
#define	__GCMANAGER_H__

#include "CrossNetRuntime/System/Object.h"
#include "CrossNetRuntime/InitOptions.h"
#include "CrossNetRuntime/System/String.h"

namespace CrossNetRuntime
{
    class GCManager
    {
    public:
        enum Generation
        {
            MAX_GENERATION = 2,
        };

        static void Setup(const ::CrossNetRuntime::InitOptions & options);
        static void Teardown();

        static void Collect(int generation, bool final);

        // The GC enable collection of one single object (without tracing pointers)
        //  This function should be used _extremely carefully_
        //  The user must be sure that no pointer is tracing to this object
        //
        //  TODO:   Implement method for delayed collection by setting a flag
        //          To detect during the tracing if this object is really not traced from another pointer
        static void CollectOneObject(::System::Object * object);

        // Tracing an object
        static CROSSNET_FINLINE
        void Trace(System::Object * object, unsigned char currentMark)
        {
            if (object == NULL)
            {
                return;
            }
            // One possible cache miss here
            if (object->__GetMark__() == currentMark)
            {
                // Already traced, skip this step
                return;
            }
            // Tell that the pointer has been traced
            object->m__AllFlags__ &= 0xffffff00;
            object->m__AllFlags__ |= currentMark;

            // Now trace all the other pointers
            // One possible cache miss here to get the VTable
            // And another one to access the corresponding method
            // Note that if we are calling the same types over and over, the number of cache misses will be reduced

            // There are ways to improve that...
            object->__Trace__(currentMark);
        }

        // Specialization for strings (to speed things up a bit)
        static CROSSNET_FINLINE
        void Trace(System::String * str, unsigned char currentMark)
        {
            if (str == NULL)
            {
                return;
            }
            // Tell that the pointer has been traced (no need to read the mark as we are not tracing it)
            str->m__AllFlags__ &= 0xffffff00;
            str->m__AllFlags__ |= currentMark;
        }

        // Tracing an interface (that is actually pointing to an object)
        static CROSSNET_FINLINE
        void Trace(::CrossNetRuntime::IInterface * interface, unsigned char currentMark)
        {
            ::System::Object * object = reinterpret_cast<::System::Object *>(interface);
            Trace(object, currentMark);
        }

        static void CheckCollecting(::System::Object * object);

        static int GetNumCollections();
        static double GetNumSecondsInGcManager();
        static double GetNumSecondsInTracingPermanent();
        static double GetNumSecondsInTracingStack();
        static double GetNumSecondsInTracingStatics();
        static double GetNumSecondsInCollect();

        static void SetTopOfStack();

    private:
        static void TraceStack(unsigned char mark);
        static bool ValidateRoot(void * value, unsigned char mark);
        static void ValidateRoot2(void * value, unsigned char mark);

        static unsigned char                sCurrentMarker;
        static bool                         sCollecting;
        static int                          sNumCollections;
        static double                       sNumSecondsInGcManager;
        static double                       sNumSecondsInTracingPermanent;
        static double                       sNumSecondsInTracingStack;
        static double                       sNumSecondsInTracingStatics;
        static double                       sNumSecondsInCollect;
        static void *                       sTopOfStack;
    };
}

#endif

