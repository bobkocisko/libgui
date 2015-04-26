#pragma once 
#include "Element.h"
#include "Control.h"

namespace libgui
{
	class ElementManager
	{
	public:
		void SetRoot(shared_ptr<Element>);

		shared_ptr<Element> GetRoot();

		bool NotifyMouseMove(int x, int y);
		bool NotifyMouseDown(int x, int y);
		bool NotifyMouseUp(int x, int y);

		void RequestCapture(shared_ptr<Control>);
		void ReleaseCapture();
	private:
		shared_ptr<Control> m_capturedControl = nullptr;
		
		// The active control is the control that was most
		// recently sent a NotifyEnter nofication.
		shared_ptr<Control> m_activeControl = nullptr;

		shared_ptr<Element> m_root = nullptr;
	};
}
