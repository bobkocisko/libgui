#pragma once 
#include "Element.h"

namespace libgui
{
	class Control : public Element
	{
	public:
		// Input events
		virtual void NotifyEnter() {};
		virtual void NotifyLeave() {};
		virtual void NotifyDown() {};
		virtual void NotifyUp() {};

		void NotifyCapturing();
		void NotifyReleasingCapture();
	protected:
		bool IsCapturing();
	private:
		bool _isCapturing = false;
	};
}
