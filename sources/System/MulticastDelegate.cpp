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

#include "CrossNetRuntime/System/MulticastDelegate.h"

void * * System::MulticastDelegate::s__InterfaceMap__ = NULL;

void System::MulticastDelegate::__RegisterId__()
{
    s__InterfaceMap__ = CrossNetRuntime::InterfaceMapper::RegisterObject(sizeof(System::MulticastDelegate), NULL, 0, System::Delegate::__GetInterfaceMap__());
}

System::Delegate * System::MulticastDelegate::CombineImpl(System::Delegate * other)
{
    // TODO:    This code is NOT correct, we need to add more to it later...
    mDelegates.push_back(other);
    return (this);
}

System::Delegate * System::MulticastDelegate::RemoveImpl(System::Delegate * other)
{
    if (other == NULL)
    {
        return (this);
    }
    // We need to remove the delegate, starting by the end...
    // Tried a reverse iterator, but there is no direct way to erase a member (need a forward iterator)
    // So instead use a forward iterator...
    std::vector<System::Delegate *>::iterator it, itBegin;
    it = mDelegates.end();
    itBegin = mDelegates.begin();

    // TODO: Make a faster implementation of this...
    while (it != itBegin)
    {
        --it;
        if (other->Equals(*it))
        {
            mDelegates.erase(it);
            return (this);
        }
    }

    // Before we leave the function make sure that we are not clearing the current object
    if (other->Equals(this))
    {
        // We are clearing the current delegate...
        if (mDelegates.empty())
        {
            // And there was no more delegate anymore
            return (NULL);
        }

        // There is at least one delegate
        // let's move from this delegate to the other delegate
        // We are assuming that we "don't know" the implementation of the delegate
        // So as such, we are doing a slow process...
        // We try to keep the same order though...
        // TODO: Optimize all of this!

        it = mDelegates.begin();
        std::vector<System::Delegate *>::iterator itEnd = mDelegates.end();

        System::Delegate * newContainer;
        newContainer = *it++;

        while (it != itEnd)
        {
            newContainer->CombineImpl(*it++);
        }
        return (newContainer);
    }

    // We didn't find the corresponding value, return our current state...
    return (this);
}