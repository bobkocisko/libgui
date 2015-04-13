#pragma once 
#include "Control.h"

namespace libgui
{
	class Button : public Control, public std::enable_shared_from_this<Button>
	{
	public:
		// Input events
		void NotifyEnter();
		void NotifyLeave();
		void NotifyDown();
		void NotifyUp();

		// Output states
		bool IsHot();
		bool IsPressed();
	private:
		bool _isHot = false;
		bool _isPressed = false;
	};

}
