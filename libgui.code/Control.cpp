#include "include/Common.h"
#include "include/Control.h"

namespace libgui
{
	void Control::NotifyCapturing()
	{
		m_isCapturing = true;
	}

	void Control::NotifyReleasingCapture()
	{
		m_isCapturing = false;
	}

	const bool& Control::GetIsEnabled() const
	{
		return m_isEnabled;
	}

	void Control::SetIsEnabled(bool isEnabled)
	{
		m_isEnabled = isEnabled;
	}

	void Control::ResetArrangement()
	{
		Element::ResetArrangement();

		// Just like the m_isVisible property in Element,
		// m_isEnabled gets reset each time before we arrange
		// and so it must be updated every time later in
		// the arrange cycle.
		m_isEnabled = true;
	}

	bool Control::IsCapturing()
	{
		return m_isCapturing;
	}
}
