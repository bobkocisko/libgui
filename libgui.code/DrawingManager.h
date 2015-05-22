#pragma once
#include "Structs.h"

using namespace std;

namespace libgui
{
	class DrawingManager
	{
	public:
		static DrawingManager* Get()
		{
			static DrawingManager drawingManager;
			return &drawingManager;
		}

		void SetStartClippingCallback(const function<void(Rect4)>&);
		const function<void(Rect4)>& GetStartClippingCallback();

		void SetStopClippingCallback(const function<void()>&);
		const function<void()>& GetStopClippingCallback();

	private:
		DrawingManager() {}
		DrawingManager(DrawingManager const&) = delete;
		void operator=(DrawingManager const&) = delete;

		function<void(Rect4)> m_startClippingCallback;
		function<void()> m_stopClippingCallback;
	};
}

