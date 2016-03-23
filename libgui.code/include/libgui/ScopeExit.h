//
// Created by Bob Kocisko on 2/8/16.
//

#pragma once

#include <functional>


namespace libgui
{

class ScopeExit
{
public:
    enum ExceptionHandling
    {
        // This is the default behavior
            TerminateOnException,

        // Catches and ignores any errors that occur during the exit phase
            IgnoreAnyExceptions
    };


    ScopeExit(std::function<void()> f);
    ScopeExit(ExceptionHandling exceptionHandling, std::function<void()> f);

    ~ScopeExit() noexcept;

private:
    ExceptionHandling     _exceptionHandling;
    std::function<void()> _f;
};

}