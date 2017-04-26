//
// Created by sjm-kociskobd on 2/20/16.
//

#include "libgui/IntersectionStack.h"


namespace libgui
{

const Rect4 IntersectionStack::EmptyRegion = Rect4();

void IntersectionStack::SetRegionChangedCallback(const std::function<void(const OptRegion&)>& regionChangedCallback)
{
  _regionChangedCallback = regionChangedCallback;
}

void IntersectionStack::PushRegion(const Rect4& newRegion)
{
  if (_stack.empty())
  {
    SignalChangeIfNeeded(newRegion);
    _stack.push(newRegion);
  }
  else
  {
    // Intersect the current region and push the result
    auto& currentRegion = _stack.top();

    if (currentRegion == EmptyRegion)
    {
      // The current region is empty, and intersecting empty with anything else results in empty.
      SignalChangeIfNeeded(EmptyRegion);
      _stack.push(EmptyRegion);
    }
    else
    {
      Rect4 intersection;
      intersection.left   = std::max(currentRegion.left, newRegion.left);
      intersection.top    = std::max(currentRegion.top, newRegion.top);
      intersection.right  = std::min(currentRegion.right, newRegion.right);
      intersection.bottom = std::min(currentRegion.bottom, newRegion.bottom);

      if (intersection.left < intersection.right &&
          intersection.top < intersection.bottom)
      {
        // The rectangles overlap
        SignalChangeIfNeeded(intersection);
        _stack.push(intersection);
      }
      else
      {
        // The rectangles do not overlap, so clip everything
        SignalChangeIfNeeded(EmptyRegion);
        _stack.push(EmptyRegion);
      }
    }
  }

}

void IntersectionStack::PopRegion()
{
  _stack.pop();

  SignalChangeIfNeeded(GetCurrentRegion());
}

void IntersectionStack::OnRegionChanged(const OptRegion& region)
{
  if (_regionChangedCallback)
  {
    _regionChangedCallback(region);
  }
}

void IntersectionStack::SignalChangeIfNeeded(const OptRegion& region)
{
  if (_lastNotification)
  {
    // Check if this region differs from the last region notified
    auto& lastNotification = _lastNotification.get();
    if (lastNotification != region)
    {
      OnRegionChanged(region);
    }
  }
  else
  {
    // There wasn't any previous notification
    OnRegionChanged(region);
  }

  _lastNotification = region;
}

IntersectionStack::OptRegion IntersectionStack::GetCurrentRegion()
{
  if (_stack.empty())
  {
    return boost::none;
  }
  else
  {
    return _stack.top();
  }
}

}