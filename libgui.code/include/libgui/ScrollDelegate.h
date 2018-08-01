#pragma once

#include <functional>

namespace libgui
{
class ScrollDelegate
{
public:
  virtual ~ScrollDelegate()
  {
  }

  virtual double GetCurrentOffsetPercent() = 0;
  virtual double GetThumbSizePercent() = 0;

  virtual void WhenThumbDataChanges(const std::function<void()>& handler) = 0;

  virtual void MoveToOffsetPercent(double offset_percent, bool notify_thumb) = 0;

  void LimitToBounds(double& offsetPercent)
  {
    offsetPercent = std::max(0.0, offsetPercent);
    offsetPercent = std::min(1.0 - GetThumbSizePercent(), offsetPercent);
  }

};
}

