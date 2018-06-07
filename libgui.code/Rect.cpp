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

bool Rect4::Intersects(const Rect4& other) const
{
  // Thanks to http://stackoverflow.com/a/306332/4307047 for the rectangle
  // intersection logic but including equality with each operator so that
  // identical rectangles would succeed, and also flipping the comparisons for
  // top and bottom since we're using top-down coordinates
  return (left <= other.right && right >= other.left &&
          top <= other.bottom && bottom >= other.top);

}

bool Rect4::ContainsVertically(const Rect4& other) const
{
  return top <= other.top && bottom >= other.bottom;
}

bool Rect4::ContainsHorizontally(const Rect4& other) const
{
  return left <= other.left && right >= other.right;
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

void Rect4::ExcludeWith(const Rect4& other)
{
  if (!Intersects(other))
  {
    // Nothing to do
    return;
  }

  auto containedByVertically = other.ContainsVertically(*this);
  auto containedByHorizontally = other.ContainsHorizontally(*this);

  if (containedByVertically && containedByHorizontally)
  {
    // Fully contained by other: empty result
    *this = Rect4();
  }
  else if (containedByVertically)
  {
    if (other.left < left)
    {
      left = other.right;
    }
    else
    {
      right = other.left;
    }
  }
  else if (containedByHorizontally)
  {
    if (other.top < top)
    {
      top = other.bottom;
    }
    else
    {
      bottom = other.top;
    }
  }
  else
  {
    // Just overlapping.  No containment

    // So we can either exclude on the horizontal or vertical dimension.
    // It makes the most sense to exclude on the dimension that leaves the
    // largest rectangle at the end, so we'll try it both ways and pick
    // the largest one to apply.

    Rect4 hExclude = *this;
    if (other.left <= hExclude.left)
    {
      if (other.right >= hExclude.left)
      {
        hExclude.left = other.right + Epsilon;
      }
    }
    else
    {
      if (other.left <= hExclude.right)
      {
        hExclude.right = other.left - Epsilon;
      }
    }

    Rect4 vExclude = *this;
    if (other.top <= vExclude.top)
    {
      if (other.bottom >= vExclude.top)
      {
        vExclude.top = other.bottom + Epsilon;
      }
    }
    else
    {
      if (other.top <= bottom)
      {
        vExclude.bottom = other.top - Epsilon;
      }
    }

    if (hExclude.Area() >= vExclude.Area())
    {
      *this = hExclude;
    }
    else
    {
      *this = vExclude;
    }
  }
}

double Rect4::Area() const
{
  return (right - left) * (bottom - top);
}

}