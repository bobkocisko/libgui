#include "libgui/Common.h"
#include "libgui/ElementManager.h"
#include "libgui/Location.h"

namespace libgui
{
void ElementManager::SetRoot(shared_ptr<Element> element)
{
    _root = element;
    element->SetElementManager(shared_from_this());
}

shared_ptr<Element> ElementManager::GetRoot()
{
    return _root;
}

//	void ElementManager::RequestCapture(shared_ptr<Control> control)
//	{
//		_capturedControl = control;
//		control->NotifyCapturing();
//
//		if (_systemCaptureCallback)
//		{
//			_systemCaptureCallback(true);
//		}
//	}
//
//	void ElementManager::ReleaseCapture()
//	{
//		if (auto cc = _capturedControl.lock())
//		{
//			cc->NotifyReleasingCapture();
//		}
//		_capturedControl.reset();
//
//		if (_systemCaptureCallback)
//		{
//			_systemCaptureCallback(false);
//		}
//	}

const function<void(bool)>& ElementManager::GetSystemCaptureCallback() const
{
    return _systemCaptureCallback;
}

void ElementManager::SetSystemCaptureCallback(const function<void(bool)>& systemCaptureCallback)
{
    _systemCaptureCallback = systemCaptureCallback;
}

//	bool ElementManager::NotifyMouseMove(int x, int y)
//	{
//		return NotifyMove(x, y, true);
//	}
//
//	bool ElementManager::NotifyMouseDown(int x, int y)
//	{
//		bool needsUpdate = false;
//
//		// Just in case the mouse move event was never propagated
//		// for this x and y position
//		needsUpdate = NotifyMouseMove(x, y);
//
//		auto ac = _activeControl.lock();
//
//		if (ac)
//		{
//			Location point{ x, y };
//			ac->NotifyMouseDown(point);
//			needsUpdate = true;
//		}
//
//		return needsUpdate;
//	}
//
//	bool ElementManager::NotifyMouseUp(int x, int y)
//	{
//		return NotifyUp(x, y, true);
//	}
//
//	bool ElementManager::NotifyMove(InputId inputId, Point point)
//	{
//		return NotifyMove(touchX, point, false);
//	}
//
//	bool ElementManager::NotifyDown(InputId inputId, Point point)
//	{
//		bool needsUpdate = false;
//
//		// Make sure that previous mouse activity is ignored when a touch down begins
//		if (auto ac = _activeControl.lock())
//		{
//			ac->NotifyMouseLeave();
//			_activeControl.reset();
//		}
//		if (auto cc = _capturedControl.lock())
//		{
//			cc->NotifyMouseLeave();
//			ReleaseCapture();
//		}
//
//		Location point{ touchX, touchY };
//
//		auto elementAtPoint = _root->GetElementAtPoint(point);
//
//		auto control = dynamic_pointer_cast<Control> (elementAtPoint);
//		if (control && control->GetIsEnabled() && control->GetIsVisible())
//		{
//			control->NotifyTouchDown(point);
//			_activeControl = control;
//			needsUpdate    = true;
//		}
//
//		return needsUpdate;
//	}
//
//	bool ElementManager::NotifyUp(InputId inputId, Point point)
//	{
//		return NotifyUp(touchX, touchY, false);
//	}
//
//	bool ElementManager::NotifyMove(int x, int y, bool isMouse)
//	{
//		bool needsUpdate = false, tempNeedsUpdate;
//
//		Location point{ x, y };
//
//		auto elementAtPoint = _root->GetElementAtPoint(point);
//
//		auto ac = _activeControl.lock();
//		auto cc = _capturedControl.lock();
//
//		if (ac)
//		{
//			if (elementAtPoint != ac)
//			{
//				isMouse ? ac->NotifyMouseLeave() : ac->NotifyTouchLeave();
//				_activeControl.reset();
//				ac = nullptr;
//				needsUpdate = true;
//			}
//		}
//
//		auto control = dynamic_pointer_cast<Control> (elementAtPoint);
//		if (control && ac != control)
//		{
//			if (ac)
//			{
//				isMouse ? ac->NotifyMouseLeave() : ac->NotifyTouchLeave();
//				needsUpdate = true;
//			}
//
//			if (control->GetIsEnabled() && control->GetIsVisible())
//			{
//				if (!cc || control == cc)
//				{
//					isMouse ? control->NotifyMouseEnter() : control->NotifyTouchEnter();
//					_activeControl = control;
//					needsUpdate    = true;
//				}
//			}
//		}
//
//		if (ac)
//		{
//			isMouse ? ac->NotifyMouseMove(point, tempNeedsUpdate) : ac->NotifyTouchMove(point, tempNeedsUpdate);
//			if (tempNeedsUpdate)
//			{
//				needsUpdate = true;
//			}
//		}
//		else if (cc)
//		{
//			isMouse ? cc->NotifyMouseMove(point, tempNeedsUpdate) : cc->NotifyTouchMove(point, tempNeedsUpdate);
//			if (tempNeedsUpdate)
//			{
//				needsUpdate = true;
//			}
//		}
//
//		return needsUpdate;
//	}
//
//	bool ElementManager::NotifyUp(int x, int y, bool isMouse)
//	{
//		bool needsUpdate = false;
//
//		// Just in case the mouse move event was never propagated
//
//		// for this x and y position
//
//		needsUpdate = isMouse ? NotifyMouseMove(x, y) : NotifyMove((InputIdentifier(0)), y);
//
//		auto cc = _capturedControl.lock();
//
//		if (cc)
//		{
//			Location point{ x, y };
//			if (isMouse)
//			{
//				cc->NotifyMouseUp(point);
//			}
//			else
//			{
//				cc->NotifyTouchUp(point);
//				_activeControl.reset();
//			}
//
//			needsUpdate = true;
//		}
//
//		return needsUpdate;
//	}


bool ElementManager::NotifyMove(InputId inputId, Point point)
{
    auto activeInput = GetActiveInput(inputId);

    auto elementAtPoint = _root->GetElementAtPoint(point);

    return activeInput->NotifyMove(point, elementAtPoint);

}

Input* ElementManager::GetActiveInput(const InputId& inputId)
{
    if (inputId >= _activeInputs.size())
    {
        _activeInputs.resize((size_t)inputId + 1, nullptr);
    }

    auto result = _activeInputs[inputId];
    if (nullptr == result)
    {
        result = new Input(inputId);
        _activeInputs[inputId] = result;
    }

    return result;
}

bool ElementManager::NotifyDown(InputId inputId, Point point)
{
    return false;
}

bool ElementManager::NotifyUp(InputId inputId, Point point)
{
    return false;
}
}
