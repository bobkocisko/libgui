#include "libgui/Control.h"

#include "libgui/ElementManager.h"

namespace libgui
{

Control::~Control()
{
  auto em = GetElementManager();
  if (em)
  {
    em->NotifyControlIsBeingDestroyed(this);
  }
}

void Control::PreNotifyInput(InputAction inputAction,
                             InputType inputType,
                             Point point,
                             bool& notify)
{
  notify = true;
}

void Control::NotifyInput(InputType inputType, InputAction inputAction, Point point)
{
}

bool Control::HasActiveInput() const
{
  return _hasActiveInput;
}

void Control::SetHasActiveInput(bool hasActiveInput)
{
  _hasActiveInput = hasActiveInput;
}

}
