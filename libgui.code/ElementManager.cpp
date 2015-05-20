#include "Precompiled.h"
#include "ElementManager.h"

namespace libgui
{
	void ElementManager::SetRoot(shared_ptr<Element> element)
	{
		m_root = element;
		element->SetElementManager(shared_from_this());
	}

	shared_ptr<Element> ElementManager::GetRoot()
	{
		return m_root;
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

		Location point{ x, y };

		auto elementAtPoint = m_root->GetElementAtPoint(point);

		if (m_activeControl)
		{
			if (elementAtPoint != m_activeControl)
			{
				m_activeControl->NotifyLeave();
				if (!m_capturedControl)
				{
					m_activeControl = nullptr;
				}
				needsUpdate = true;
			}
		}

		auto control = dynamic_pointer_cast<Control> (elementAtPoint);
		if (control && m_activeControl != control)
		{
			if (m_activeControl)
			{
				m_activeControl->NotifyLeave();
			}

			if (!m_capturedControl || control == m_capturedControl)
			{
				control->NotifyEnter();
				m_activeControl = control;
			}
			needsUpdate = true;
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
