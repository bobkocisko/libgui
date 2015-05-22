#include "Precompiled.h"
#include "DrawingManager.h"

namespace libgui
{
	void DrawingManager::SetStartClippingCallback(const function<void(Rect4)>& startClippingCallback)
	{
		m_startClippingCallback = startClippingCallback;
	}

	const function<void(Rect4)>& DrawingManager::GetStartClippingCallback()
	{
		return m_startClippingCallback;
	}

	void DrawingManager::SetStopClippingCallback(const function<void()>& stopClippingCallback)
	{
		m_stopClippingCallback = stopClippingCallback;
	}

	const function<void()>& DrawingManager::GetStopClippingCallback()
	{
		return m_stopClippingCallback;
	}
}