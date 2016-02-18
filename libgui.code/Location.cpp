//
// Created by Bob Kocisko on 10/8/15.
//

#include "libgui/Location.h"

Location::Location()
    : Location(0.0, 0.0)
{
}

Location::Location(const Length& x, const Length& y)
    : x(x), y(y)
{
}
Location::Location(Length& x, Length& y)
    : x(x), y(y)
{
}
