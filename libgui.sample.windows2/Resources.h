#pragma once

namespace libgui_sample_windows2
{
	class Resources
	{
	public:
		virtual ~Resources() { }

		virtual HRESULT Create(ID2D1Factory*, ID2D1HwndRenderTarget*) = 0;
		virtual void Discard() = 0;
	};
}