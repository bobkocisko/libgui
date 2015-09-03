#pragma once 
#include "Control.h"

namespace libgui
{
	class Button : public Control
	{
	public:
		// Input events
		void NotifyMouseEnter() override;
		void NotifyMouseLeave() override;
		void NotifyMouseDown(Location location) override;
		void NotifyMouseUp(Location location) override;

		void NotifyTouchEnter() override;
		void NotifyTouchLeave() override;
		void NotifyTouchDown(Location location) override;
		void NotifyTouchUp(Location location) override;

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

		void NotifyEnter(bool isMouse);
		void NotifyLeave(bool isMouse);
		void NotifyDown(bool isMouse);
		void NotifyUp(bool isMouse);
	};

}
