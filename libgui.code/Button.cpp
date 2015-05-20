#include "Precompiled.h"
#include "Button.h"
#include "ElementManager.h"

namespace libgui
{
	void Button::NotifyEnter()
	{
		_isOver = true;

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
		_isOver = false;

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

		GetElementManager()->RequestCapture(dynamic_pointer_cast<Button>(shared_from_this()));
	}

	void Button::NotifyUp()
	{
		_isPressed = false;
		if (IsCapturing())
		{
			OnClick();

			if (!_isOver)
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
