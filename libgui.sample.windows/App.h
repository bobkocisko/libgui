#pragma once
#include "Resources.h"
#include "libgui.code\ElementManager.h"
#include "enums.h";

using namespace libgui;

namespace libgui_sample_windows
{
	class App
	{
	public:
		App();
		~App();

		// Register the window class and call methods for instantiating drawing resources
		HRESULT Initialize();

		// Process and dispatch messages
		void RunMessageLoop();

	private:
		void DrawBorder(double, ID2D1Brush*, shared_ptr<Element>);
		void DrawBorder(int, double, ID2D1Brush*, shared_ptr<Element>);

		// Initialize device-independent resources.
		HRESULT CreateDeviceIndependentResources();

		// Initialize device-dependent resources.
		HRESULT CreateDeviceResources();

		// Release device-dependent resource.
		void DiscardDeviceResources();

		// Draw content.
		HRESULT OnRender();

		// Resize the render target.
		void OnResize(
			UINT width,
			UINT height
			);

		// The windows procedure.
		static LRESULT CALLBACK WndProc(
			HWND hWnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam
			);

		HWND m_hwnd;
		ID2D1Factory* m_pDirect2dFactory;
		ID2D1HwndRenderTarget* m_pRenderTarget;

		shared_ptr<ElementManager> m_elementManager = make_shared<ElementManager>();
		std::vector<Resources*> m_resources;
	};

}