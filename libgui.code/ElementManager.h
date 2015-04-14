#pragma once 
#include "Element.h"
#include "Control.h"

namespace libgui
{
	class ElementManager
	{
	public:
		// Singleton stuff
		static ElementManager* Get();

		void SetRoot(shared_ptr<Element>);

		shared_ptr<Element> GetRoot();

		bool NotifyMouseMove(int x, int y);
		bool NotifyMouseDown(int x, int y);
		bool NotifyMouseUp(int x, int y);

		void RequestCapture(shared_ptr<Control>);
		void ReleaseCapture();
	private:
		// Singleton stuff
		ElementManager() {};
		// Singleton stuff
		ElementManager(ElementManager const&) = delete;
		// Singleton stuff
		void operator=(ElementManager const&) = delete;

		shared_ptr<Control> m_capturedControl = nullptr;
		
		// The active control is the control that was most
		// recently sent a NotifyEnter nofication.
		shared_ptr<Control> m_activeControl = nullptr;

		shared_ptr<Element> m_root = nullptr;
	};
}
