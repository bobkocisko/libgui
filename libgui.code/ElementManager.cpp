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
		bool needsUpdate = false, tempNeedsUpdate;

		Location point{ x, y };

		auto elementAtPoint = m_root->GetElementAtPoint(point);

		auto ac = m_activeControl.lock();
		auto cc = m_capturedControl.lock();

		if (ac)
		{
			if (elementAtPoint != ac)
			{
				ac->NotifyMouseLeave();
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
				ac->NotifyMouseLeave();
			}

			if (!cc || control == cc)
			{
				control->NotifyMouseEnter();
				m_activeControl = control;
			}
			needsUpdate = true;
		}

		if (ac)
		{
			ac->NotifyMouseMove(point, tempNeedsUpdate);
			if (tempNeedsUpdate)
			{
				needsUpdate = true;
			}
		}
		else if (cc)
		{
			cc->NotifyMouseMove(point, tempNeedsUpdate);
			if (tempNeedsUpdate)
			{
				needsUpdate = true;
			}
		}

		return needsUpdate;
	}

	bool ElementManager::NotifyMouseDown(int x, int y)
	{
		bool needsUpdate = false;

		// Just in case the mouse move event was never propagated
		// for this x and y position
		needsUpdate = NotifyMouseMove(x, y);

		auto ac = m_activeControl.lock();

		if (ac)
		{
			Location point{ x, y };
			ac->NotifyMouseDown(point);
			needsUpdate = true;
		}

		return needsUpdate;
	}

	bool ElementManager::NotifyMouseUp(int x, int y)
	{
		bool needsUpdate = false;

		// Just in case the mouse move event was never propagated
		// for this x and y position
		needsUpdate = NotifyMouseMove(x, y);

		auto ac = m_activeControl.lock();

		if (ac)
		{
			Location point{ x, y };
			ac->NotifyMouseUp(point);
			needsUpdate = true;
		}

		return needsUpdate;
	}

	bool ElementManager::NotifyTouchMove(double touchX, double touchY)
	{
		bool needsUpdate = false, tempNeedsUpdate;

		Location point{ touchX, touchY };

		auto elementAtPoint = m_root->GetElementAtPoint(point);

		auto cc = m_capturedControl.lock();

		if (cc)
		{
			if (elementAtPoint != cc)
			{
				cc->NotifyTouchLeave();
				needsUpdate = true;
			}
		}

		auto control = dynamic_pointer_cast<Control> (elementAtPoint);
		if (control && cc != control)
		{
			if (control == cc)
			{
				control->NotifyTouchEnter();
				needsUpdate = true;
			}
		}

		if (cc)
		{
			cc->NotifyMouseMove(point, tempNeedsUpdate);
			if (tempNeedsUpdate)
			{
				needsUpdate = true;
			}
		}

		return needsUpdate;
	}

	bool ElementManager::NotifyTouchDown(double touchX, double touchY)
	{
		bool needsUpdate = false;

		Location point{ touchX, touchY };

		auto elementAtPoint = m_root->GetElementAtPoint(point);

		auto control = dynamic_pointer_cast<Control> (elementAtPoint);
		if (control)
		{
			control->NotifyTouchDown(point);
			needsUpdate = true;
		}

		return needsUpdate;
	}

	bool ElementManager::NotifyTouchUp(double touchX, double touchY)
	{
		bool needsUpdate = false;

		// Just in case the touch move event was never propagated
		// for this x and y position
		needsUpdate = NotifyTouchMove(touchX, touchY);

		auto ac = m_activeControl.lock();

		if (ac)
		{
			Location point{ touchX, touchY };
			ac->NotifyTouchUp(point);
			needsUpdate = true;
		}

		return needsUpdate;
	}
}
