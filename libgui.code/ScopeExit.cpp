//
// Created by Bob Kocisko on 2/8/16.
//

#include "libgui/ScopeExit.h"

ScopeExit::ScopeExit(std::function<void()> f)
    : _f(f)
{

}

ScopeExit::~ScopeExit()
{
    _f();
}
