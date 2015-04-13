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

		void AddElement(shared_ptr<Element>);

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
		vector<shared_ptr<Element>> m_elements;
	};
}
