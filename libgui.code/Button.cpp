#include "libgui/Common.h"
#include "libgui/Button.h"
#include "libgui/ElementManager.h"
#include "libgui/Location.h"

namespace libgui
{
	void Button::NotifyMouseEnter()
	{
		NotifyEnter(true);
	}

	void Button::NotifyMouseLeave()
	{
		NotifyLeave(true);
	}

	void Button::NotifyMouseDown(Location location)
	{
		NotifyDown(true);
	}

	void Button::NotifyMouseUp(Location location)
	{
		NotifyUp(true);
	}

	void Button::NotifyTouchEnter()
	{
		NotifyEnter(false);
	}

	void Button::NotifyTouchLeave()
	{
		NotifyLeave(false);
	}

	void Button::NotifyTouchDown(Location location)
	{
		NotifyDown(false);
	}

	void Button::NotifyTouchUp(Location location)
	{
		NotifyUp(false);
	}

	void Button::NotifyEnter(bool isMouse)
	{
		if (isMouse || IsCapturing())
		{
			_isOver = true;
		}

		if (IsCapturing())
		{
			_isPressed = true;
		}
		else if (isMouse)
		{
			_isHot = true;
		}
	}

	void Button::NotifyLeave(bool isMouse)
	{
		_isOver = false;

		if (IsCapturing())
		{
			_isPressed = false;
		}
		else if (isMouse)
		{
			_isHot = false;
		}
	}

	void Button::NotifyDown(bool isMouse)
	{
		if (isMouse)
		{
			_isHot = true;
		}
		else
		{
			_isOver = true;
		}

		_isPressed = true;

		GetElementManager()->RequestCapture(dynamic_pointer_cast<Button>(shared_from_this()));
	}

	void Button::NotifyUp(bool isMouse)
	{
		_isPressed = false;
		if (IsCapturing())
		{
			if (_isOver)
			{
				OnClick();
			}
			else if (isMouse)
			{
				_isHot = false;
			}

			GetElementManager()->ReleaseCapture();
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

	void Button::OnClick()
	{
		// Default implementation: invoke the callback
		if (m_clickCallback)
		{
			m_clickCallback(dynamic_pointer_cast<Button>(shared_from_this()));
		}
	}

	void Button::SetClickCallback(function<void(shared_ptr<Button>)> clickCallback)
	{
		m_clickCallback = clickCallback;
	}
}
