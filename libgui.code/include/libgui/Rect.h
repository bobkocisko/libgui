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
  static constexpr double Epsilon = 0.00001;

  Rect4();
  Rect4(double left, double top, double right, double bottom);

  double left;
  double top;
  double right;
  double bottom;

  bool IsEmpty();

  bool operator==(const Rect4& other) const;
  bool operator!=(const Rect4& other) const;

  bool Intersects(const Rect4& other) const;

  bool ContainsVertically(const Rect4& other) const;
  bool ContainsHorizontally(const Rect4& other) const;

  // Changes this Rect4 to become the intersection of itself
  // and the specified Rect4.  The two Rect4's must intersect
  // or else the result will be a deformed Rect4.
  void IntersectWith(const Rect4& other);

  // Changes this Rect4 to become the exclusion of itself
  // and the specified Rect4.  If there is no intersection
  // then it stays exactly the same.  If this is fully
  // contained in the other, then this will become empty.
  // If this contains the other, then it will pick an arbitrary
  // side to exclude towards.
  void ExcludeWith(const Rect4& other);

  double Area() const;
};
}