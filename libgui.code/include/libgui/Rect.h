#pragma once

#include "Size.h"
#include "Location.h"
namespace libgui
{

struct Rect
{
    Rect(Location, Size);

    Location location;
    Size     size;

    double GetLeft();
    double GetTop();
    double GetRight();
    double GetBottom();
};

struct Rect4
{
    Rect4();
    Rect4(double left, double top, double right, double bottom);

    double left;
    double top;
    double right;
    double bottom;

    bool IsEmpty();

    bool operator==(const Rect4& other) const;
    bool operator!=(const Rect4& other) const;

    // Changes this Rect4 to become the intersection of itself
    // and the specified Rect4.  The two Rect4's must intersect
    // or else the result will be a deformed Rect4.
    void IntersectWith(const Rect4& other);
};
}