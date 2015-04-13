#include "Precompiled.h"
#include "Button.h"
#include "ElementManager.h"

namespace libgui
{
	void Button::NotifyEnter()
	{
		if (IsCapturing())
		{
			_isPressed = true;
		}
		else
		{
			_isHot = true;
		}
	}

	void Button::NotifyLeave()
	{
		if (IsCapturing())
		{
			_isPressed = false;
		}
		else
		{
			_isHot = false;
		}
	}

	void Button::NotifyDown()
	{
		_isHot = true;
		_isPressed = true;

		ElementManager::Get()->RequestCapture(shared_from_this());
	}

	void Button::NotifyUp()
	{
		_isPressed = false;
		if (IsCapturing())
		{
			_isHot = false;
			ElementManager::Get()->ReleaseCapture();
		}
	}

	bool Button::IsHot()
	{
		return _isHot;
	}

	bool Button::IsPressed()
	{
		return _isPressed;
	}
}
