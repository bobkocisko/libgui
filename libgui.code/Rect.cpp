#include "libgui/Rect.h"
#include "libgui/Size.h"
#include "libgui/Location.h"
#include <algorithm>

namespace libgui
{
Rect::Rect(Location location, Size size)
    : location(location),
      size(size)
{
}

double Rect::GetLeft()
{
    return location.x;
}

double Rect::GetTop()
{
    return location.y;
}

double Rect::GetRight()
{
    return location.x + size.width;
}

double Rect::GetBottom()
{
    return location.y + size.height;
}

Rect4::Rect4(double left, double top, double right, double bottom)
    : left(left),
      top(top),
      right(right),
      bottom(bottom)
{
}

Rect4::Rect4()
    : left(0),
      top(0),
      right(0),
      bottom(0)
{

}

bool Rect4::operator==(const Rect4& other) const
{
    return left == other.left &&
           top == other.top &&
           right == other.right &&
           bottom == other.bottom;
}

bool Rect4::operator!=(const Rect4& other) const
{
    return !operator==(other);
}

bool Rect4::IsEmpty()
{
    return left == 0 &&
           top == 0 &&
           right == 0 &&
           bottom == 0;
}

void Rect4::IntersectWith(const Rect4& other)
{
    // This algorithm assumes that this and other actually do
    // intersect.  If they do not then this will become a deformed rectangle.
    left   = std::max(left, other.left);
    top    = std::max(top, other.top);
    right  = std::min(right, other.right);
    bottom = std::min(bottom, other.bottom);
}
}