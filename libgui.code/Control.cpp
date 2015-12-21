#include "libgui/Common.h"
#include "libgui/Control.h"

namespace libgui
{
const bool& Control::GetIsEnabled() const
{
    return _isEnabled;
}

void Control::SetIsEnabled(bool isEnabled)
{
    _isEnabled = isEnabled;
}

void Control::ResetArrangement()
{
    Element::ResetArrangement();

    // Just like the _isVisible property in Element,
    // _isEnabled gets reset each time before we arrange
    // and so it must be updated every time later in
    // the arrange cycle.
    _isEnabled = true;
}

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
