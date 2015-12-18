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

	void virtual Control::PreNotifyInput(InputAction inputAction,
										 InputType inputType,
										 Point point,
										 bool& updateScreen,
										 bool& notify)
	{
		updateScreen = (InputAction::Move != inputAction);
		notify = true;
	}

	void virtual Control::NotifyInput(InputAction inputAction, InputType inputType, Point point, bool& updateScreen)
	{
		updateScreen = (InputAction::Move != inputAction);
	}
}
