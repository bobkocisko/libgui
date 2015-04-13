#include "Precompiled.h"
#include "ElementManager.h"

namespace libgui
{
	ElementManager* ElementManager::Get()
	{
		static ElementManager instance;
		return &instance;
	};

	void ElementManager::AddElement(shared_ptr<Element> element)
	{
		m_elements.push_back(element);
	}

	void ElementManager::RequestCapture(shared_ptr<Control> control)
	{
		m_capturedControl = control;
		control->NotifyCapturing();
	}

	void ElementManager::ReleaseCapture()
	{
		m_capturedControl->NotifyReleasingCapture();
		m_capturedControl = nullptr;
	}
}
