#include "libgui/Common.h"
#include "libgui/Control.h"

namespace libgui
{
void Control::PreNotifyInput(InputAction inputAction,
                             InputType inputType,
                             Point point,
                             bool& updateScreen,
                             bool& notify)
{
    updateScreen = (InputAction::Move != inputAction);
    notify       = true;
}

void Control::NotifyInput(InputType inputType, InputAction inputAction, Point point, bool& updateScreen)
{
    updateScreen = (InputAction::Move != inputAction);
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
