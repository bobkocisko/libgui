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
		return NotifyMove(x, y, true);
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
		return NotifyUp(x, y, true);
	}

	bool ElementManager::NotifyTouchMove(double touchX, double touchY)
	{
		return NotifyMove(touchX, touchY, false);
	}

	bool ElementManager::NotifyTouchDown(double touchX, double touchY)
	{
		bool needsUpdate = false;

		// Make sure that previous mouse activity is ignored when a touch down begins
		if (auto ac = m_activeControl.lock())
		{
			ac->NotifyMouseLeave();
			m_activeControl.reset();
		}
		if (auto cc = m_capturedControl.lock())
		{
			cc->NotifyMouseLeave();
			ReleaseCapture();
		}

		Location point{ touchX, touchY };

		auto elementAtPoint = m_root->GetElementAtPoint(point);

		auto control = dynamic_pointer_cast<Control> (elementAtPoint);
		if (control)
		{
			control->NotifyTouchDown(point);
			m_activeControl = control;
			needsUpdate = true;
		}

		return needsUpdate;
	}

	bool ElementManager::NotifyTouchUp(double touchX, double touchY)
	{
		return NotifyUp(touchX, touchY, false);
	}

	bool ElementManager::NotifyMove(int x, int y, bool isMouse)
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
				isMouse ? ac->NotifyMouseLeave() : ac->NotifyTouchLeave();
				m_activeControl.reset();
				ac = nullptr;
				needsUpdate = true;
			}
		}

		auto control = dynamic_pointer_cast<Control> (elementAtPoint);
		if (control && ac != control)
		{
			if (ac)
			{
				isMouse ? ac->NotifyMouseLeave() : ac->NotifyTouchLeave();
			}

			if (!cc || control == cc)
			{
				isMouse ? control->NotifyMouseEnter() : control->NotifyTouchEnter();
				m_activeControl = control;
			}
			needsUpdate = true;
		}

		if (ac)
		{
			isMouse ? ac->NotifyMouseMove(point, tempNeedsUpdate) : ac->NotifyTouchMove(point, tempNeedsUpdate);
			if (tempNeedsUpdate)
			{
				needsUpdate = true;
			}
		}
		else if (cc)
		{
			isMouse ? cc->NotifyMouseMove(point, tempNeedsUpdate) : cc->NotifyTouchMove(point, tempNeedsUpdate);
			if (tempNeedsUpdate)
			{
				needsUpdate = true;
			}
		}

		return needsUpdate;
	}

	bool ElementManager::NotifyUp(int x, int y, bool isMouse)
	{
		bool needsUpdate = false;

		// Just in case the mouse move event was never propagated

		// for this x and y position

		needsUpdate = isMouse ? NotifyMouseMove(x, y) : NotifyTouchMove(x, y);

		auto cc = m_capturedControl.lock();

		if (cc)
		{
			Location point{ x, y };
			if (isMouse)
			{
				cc->NotifyMouseUp(point);
			}
			else
			{
				cc->NotifyTouchUp(point);
				m_activeControl.reset();
			}

			needsUpdate = true;
		}

		return needsUpdate;
	}


}
