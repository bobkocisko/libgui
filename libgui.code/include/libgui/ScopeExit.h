//
// Created by Bob Kocisko on 2/8/16.
//

#pragma once

#include <functional>


class ScopeExit
{
public:
    ScopeExit(std::function<void()> f);
    ~ScopeExit();

private:
    std::function<void()> _f;
};



