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
		if (auto cc = m_capturedControl.lock())
		{
			cc->NotifyReleasingCapture();
		}
		m_capturedControl.reset();
	}

	bool ElementManager::NotifyMouseMove(int x, int y)
	{
		bool needsUpdate = false;

		Location point{ x, y };

		auto elementAtPoint = m_root->GetElementAtPoint(point);

		auto ac = m_activeControl.lock();
		auto cc = m_capturedControl.lock();

		if (ac)
		{
			if (elementAtPoint != ac)
			{
				ac->NotifyLeave();
				if (!cc)
				{
					m_activeControl.reset();
					ac = nullptr;
				}
				needsUpdate = true;
			}
		}

		auto control = dynamic_pointer_cast<Control> (elementAtPoint);
		if (control && ac != control)
		{
			if (ac)
			{
				ac->NotifyLeave();
			}

			if (!cc || control == cc)
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

		auto ac = m_activeControl.lock();

		if (ac)
		{
			ac->NotifyDown();
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

		auto ac = m_activeControl.lock();

		if (ac)
		{
			ac->NotifyUp();
			needsUpdate = true;
		}

		return needsUpdate;
	}

}
