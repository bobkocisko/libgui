//
// Created by Bob Kocisko on 10/8/15.
//

#include "libgui/Location.h"

namespace libgui
{

Location::Location()
    : Location(0.0, 0.0)
{
}

Location::Location(double x, double y)
    : x(x), y(y)
{
}

}