#pragma once 
#include "Element.h"

namespace libgui
{
	class Control : public Element
	{
	public:
		// Input events

		virtual void NotifyMouseEnter() {};
		virtual void NotifyMouseLeave() {};
		virtual void NotifyMouseDown(Location location) {};
		virtual void NotifyMouseUp(Location location) {};
		virtual void NotifyMouseMove(Location location, bool& updateScreen)
		{
			updateScreen = false;
		};

		virtual void PreNotifyTouchLeave(bool& notify) { notify = true; };
		virtual void PreNotifyTouchEnter(bool& notify) { notify = true; };
		virtual void PreNotifyTouchDown(Location location, bool& notify) { notify = true; };
		virtual void PreNotifyTouchUp(Location location, bool& notify) { notify = true; };
		virtual void PreNotifyTouchMove(Location location, bool& updateScreen, bool& notify)
		{
			updateScreen = false;
			notify = true;
		};

		virtual void NotifyTouchLeave() {};
		virtual void NotifyTouchEnter() {};
		virtual void NotifyTouchDown(Location location) {};
		virtual void NotifyTouchUp(Location location) {};
		virtual void NotifyTouchMove(Location location, bool& updateScreen)
		{
			updateScreen = false;
		};

		void NotifyCapturing();
		void NotifyReleasingCapture();
	protected:
		bool IsCapturing();
	private:
		bool _isCapturing = false;
	};
}
