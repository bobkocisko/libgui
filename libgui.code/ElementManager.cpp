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

		if (m_systemCaptureCallback)
		{
			m_systemCaptureCallback(true);
		}
	}

	void ElementManager::ReleaseCapture()
	{
		if (auto cc = m_capturedControl.lock())
		{
			cc->NotifyReleasingCapture();
		}
		m_capturedControl.reset();
		
		if (m_systemCaptureCallback)
		{
			m_systemCaptureCallback(false);
		}
	}

	const function<void(bool)>& ElementManager::GetSystemCaptureCallback() const
	{
		return m_systemCaptureCallback;
	}

	void ElementManager::SetSystemCaptureCallback(const function<void(bool)>& systemCaptureCallback)
	{
		m_systemCaptureCallback = systemCaptureCallback;
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

		if (ac)
		{
			if (ac->NotifyMove(point))
			{
				needsUpdate = true;
			}
		}
		else if (cc)
		{
			if (cc->NotifyMove(point))
			{
				needsUpdate = true;
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

		auto ac = m_activeControl.lock();

		if (ac)
		{
			Location point{ x, y };
			ac->NotifyDown(point);
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
			Location point{ x, y };
			ac->NotifyUp(point);
			needsUpdate = true;
		}

		return needsUpdate;
	}

}
