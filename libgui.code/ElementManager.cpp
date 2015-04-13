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

	bool ElementManager::NotifyMouseMove(int x, int y)
	{
		bool needsUpdate = false;

		if (m_activeControl)
		{
			if (!m_activeControl->HitTest( Location { x, y }))
			{
				m_activeControl->NotifyLeave();
				m_activeControl = NULL;
				needsUpdate = true;
			}
		}

		for (auto &element : m_elements)
		{
			if (element->HitTest(Location{ x, y }))
			{
				auto control = dynamic_pointer_cast<Control> (element);
				if (m_activeControl != control)
				{
					if (m_activeControl)
					{
						m_activeControl->NotifyLeave();
					}

					control->NotifyEnter();
					m_activeControl = control;
					needsUpdate = true;
				}

				break;
			}
		}

		return needsUpdate;
	}

	bool ElementManager::NotifyMouseDown(int x, int y)
	{
		bool needsUpdate;

		// Just in case the mouse move event was never propagated
		// for this x and y position
		needsUpdate = NotifyMouseMove(x, y);

		if (m_activeControl)
		{
			m_activeControl->NotifyDown();
			needsUpdate = true;
		}

		return needsUpdate;
	}

	bool ElementManager::NotifyMouseUp(int x, int y)
	{
		bool needsUpdate;

		// Just in case the mouse move event was never propagated
		// for this x and y position
		needsUpdate = NotifyMouseMove(x, y);

		if (m_activeControl)
		{
			m_activeControl->NotifyUp();
			needsUpdate = true;
		}

		return needsUpdate;
	}

}
