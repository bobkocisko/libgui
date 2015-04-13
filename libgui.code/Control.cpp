#include "Precompiled.h"
#include "Control.h"

namespace libgui
{
	void Control::NotifyCapturing()
	{
		_isCapturing = true;
	}

	void Control::NotifyReleasingCapture()
	{
		_isCapturing = false;
	}

	bool Control::IsCapturing()
	{
		return _isCapturing;
	}
}
