#pragma once 
#include "Control.h"

namespace libgui
{
	class Button : public Control
	{
	public:
		// Input events
		void NotifyEnter();
		void NotifyLeave();
		void NotifyDown(Location location);
		void NotifyUp(Location location);

		// Output states
		bool IsHot();
		bool IsPressed();
		
		// Output events
		virtual void OnClick();
		void SetClickCallback(function<void(shared_ptr<Button>)>);

	private:
		bool _isOver = false;
		bool _isHot = false;
		bool _isPressed = false;

		function<void(shared_ptr<Button>)> m_clickCallback;
	};

}
