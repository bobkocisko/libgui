#include "Precompiled.h"
#include "App.h"
#include "SharedResources.h"
#include "libgui.code\Element.h"
#include "enums.h"

namespace libgui_sample_windows
{
	App::App() :
		m_hwnd(NULL),
		m_pDirect2dFactory(NULL),
		m_pRenderTarget(NULL)
	{
		m_resources.push_back(SharedResources::Get());

		// Set up the root element to match the window size
		auto root = make_shared<Element>();
		m_elementManager->SetRoot(root);
		root->SetElementManager(m_elementManager);
		root->SetArrangeCallback([&](shared_ptr<Element> e)
		{
			D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
			e->SetLeft(0); e->SetRight(rtSize.width);
			e->SetTop(0); e->SetBottom(rtSize.height);
		});


		// Build the screen elements

		auto header = make_shared<Element>();
		{
			root->AddChild(header);
			header->SetArrangeCallback([&](shared_ptr<Element> e)
			{
				auto p = e->GetParent();
				e->SetLeft(p->GetLeft()); e->SetRight(p->GetRight());
				e->SetTop(p->GetTop()); e->SetHeight(41.0);
			});
			header->SetDrawCallback([&](shared_ptr<Element> e)
			{
				DrawBorder(1.0, SharedResources::Get()->GrayBrush, e);
			});
		}

		auto page = make_shared<Element>();
		{
			root->AddChild(page);
			page->SetArrangeCallback([&](shared_ptr<Element> e)
			{
				auto p = e->GetParent();
				auto s = e->GetPrevSibling();
				e->SetLeft(p->GetLeft()); e->SetRight(p->GetRight());
				e->SetTop(s->GetBottom()); e->SetBottom(p->GetBottom());
			});
			page->SetDrawCallback([&](shared_ptr<Element> e)
			{
				auto sr = SharedResources::Get();
				DrawBorder(LEFT | RIGHT | BOTTOM, 1.0, sr->LightGrayBrush, e);

				m_pRenderTarget->DrawLine(
					D2D1::Point2F(e->GetLeft() + 38.5, e->GetTop()),
					D2D1::Point2F(e->GetLeft() + 38.5, e->GetBottom() - 1.0),
					sr->LightRedBrush, 1.0);
				m_pRenderTarget->DrawLine(
					D2D1::Point2F(e->GetLeft() + 42.5, e->GetTop()),
					D2D1::Point2F(e->GetLeft() + 42.5, e->GetBottom() - 1.0),
					sr->LightRedBrush, 1.0);
			});

			auto pageHeader = make_shared<Element>();
			page->AddChild(pageHeader);
			pageHeader->SetArrangeCallback([&](shared_ptr<Element> e)
			{
				auto p = e->GetParent();
				e->SetLeft(p->GetLeft()); e->SetRight(p->GetRight());
				e->SetTop(p->GetTop()); e->SetHeight(40.0);
			});

			auto pageFooter = make_shared<Element>();
			page->AddChild(pageFooter);
			pageFooter->SetArrangeCallback([&](shared_ptr<Element> e)
			{
				auto p = e->GetParent();
				e->SetLeft(p->GetLeft() + 1.0); e->SetRight(p->GetRight() - 1.0);
				e->SetBottom(p->GetBottom()); e->SetHeight(40.0);
			});
			pageFooter->SetDrawCallback([&](shared_ptr<Element> e)
			{
				DrawBorder(TOP, 1.0, SharedResources::Get()->LightGrayBrush, e);
			});

			auto pageContentContainer = make_shared<Element>();
			page->AddChild(pageContentContainer);
			pageContentContainer->SetArrangeCallback([=](shared_ptr<Element> e)
			{
				auto p = e->GetParent();
				e->SetLeft(p->GetLeft() + 1.0); e->SetRight(p->GetRight() - 1.0);
				e->SetTop(pageHeader->GetBottom()); e->SetBottom(pageFooter->GetTop());
			});
		}
	}

	void App::DrawBorder(double strokeWidth, ID2D1Brush* brush, shared_ptr<Element> e )
	{
		DrawBorder(LEFT | TOP | RIGHT | BOTTOM, strokeWidth, brush, e);
	}

	void App::DrawBorder(int sides, double strokeWidth, ID2D1Brush* brush, shared_ptr<Element> e)
	{
		double halfStrokeWidth = strokeWidth / 2;

		if ((LEFT | TOP | RIGHT | BOTTOM) == sides)
		{
			// All sides: use a rectangle and draw its border
			D2D1_RECT_F rect = {};
			rect.left = e->GetLeft() + halfStrokeWidth;
			rect.right = e->GetRight() - halfStrokeWidth;
			rect.top = e->GetTop() + halfStrokeWidth;
			rect.bottom = e->GetBottom() - halfStrokeWidth;
			m_pRenderTarget->DrawRectangle(rect, brush, strokeWidth);
		}
		else
		{
			D2D1_RECT_F outerRect;
			outerRect.left = e->GetLeft();
			outerRect.right = e->GetRight();
			outerRect.top = e->GetTop();
			outerRect.bottom = e->GetBottom();
			
			D2D1_RECT_F rect = {};
			rect.left = outerRect.left + halfStrokeWidth;
			rect.right = outerRect.right - halfStrokeWidth;
			rect.top = outerRect.top + halfStrokeWidth;
			rect.bottom = outerRect.bottom - halfStrokeWidth;
			// One or more side: use lines
			if (0 < (sides & LEFT))
			{
				m_pRenderTarget->DrawLine(
					D2D1::Point2F(rect.left, outerRect.top),
					D2D1::Point2F(rect.left, outerRect.bottom),
					brush, strokeWidth);
			}
			if (0 < (sides & RIGHT))
			{
				m_pRenderTarget->DrawLine(
					D2D1::Point2F(rect.right, outerRect.top),
					D2D1::Point2F(rect.right, outerRect.bottom),
					brush, strokeWidth);
			}
			if (0 < (sides & TOP))
			{
				double left = outerRect.left;
				double right = outerRect.right;
				if (0 < (sides & LEFT)) left += strokeWidth;
				if (0 < (sides & RIGHT)) right -= strokeWidth;
				m_pRenderTarget->DrawLine(
					D2D1::Point2F(left, rect.top),
					D2D1::Point2F(right, rect.top),
					brush, strokeWidth);
			}
			if (0 < (sides & BOTTOM))
			{
				double left = outerRect.left;
				double right = outerRect.right;
				if (0 < (sides & LEFT)) left += strokeWidth;
				if (0 < (sides & RIGHT)) right -= strokeWidth;
				m_pRenderTarget->DrawLine(
					D2D1::Point2F(left, rect.bottom),
					D2D1::Point2F(right, rect.bottom),
					brush, strokeWidth);
			}
		}
	}

	App::~App()
	{
		SafeRelease(&m_pDirect2dFactory);

		DiscardDeviceResources();
	}


	void App::RunMessageLoop()
	{
		MSG msg;

		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	HRESULT App::Initialize()
	{
		HRESULT hr;

		// Initialize device-indpendent resources, such
		// as the Direct2D factory.
		hr = CreateDeviceIndependentResources();

		if (SUCCEEDED(hr))
		{
			// Register the window class.
			WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = App::WndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = sizeof(LONG_PTR);
			wcex.hInstance = HINST_THISCOMPONENT;
			wcex.hbrBackground = NULL;
			wcex.lpszMenuName = NULL;
			wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
			wcex.lpszClassName = L"D2DApp";

			RegisterClassEx(&wcex);


			// Because the CreateWindow function takes its size in pixels,
			// obtain the system DPI and use it to scale the window size.
			FLOAT dpiX, dpiY;

			// The factory returns the current system DPI. This is also the value it will use
			// to create its own windows.
			m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);


			// Create the window.
			m_hwnd = CreateWindow(
				L"D2DApp",
				L"Direct2D Demo App",
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				static_cast<UINT>(ceil(252.f * dpiX / 96.f)),
				static_cast<UINT>(ceil(280.f * dpiY / 96.f)),
				NULL,
				NULL,
				HINST_THISCOMPONENT,
				this
				);
			hr = m_hwnd ? S_OK : E_FAIL;
			if (SUCCEEDED(hr))
			{
				// Make the window borderless and captionless before showing it
				LONG lStyle = GetWindowLong(m_hwnd, GWL_STYLE);
				lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU |
					WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
				SetWindowLong(m_hwnd, GWL_STYLE, lStyle);
				SetWindowPos(m_hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

				ShowWindow(m_hwnd, SW_SHOWNORMAL);
				UpdateWindow(m_hwnd);
			}
		}

		return hr;
	}

	HRESULT App::CreateDeviceIndependentResources()
	{
		HRESULT hr = S_OK;

		// Create a Direct2D factory.
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

		return hr;
	}

	HRESULT App::CreateDeviceResources()
	{
		HRESULT hr = S_OK;

		if (!m_pRenderTarget)
		{
			RECT rc;
			GetClientRect(m_hwnd, &rc);

			D2D1_SIZE_U size = D2D1::SizeU(
				rc.right - rc.left,
				rc.bottom - rc.top
				);

			// Create a Direct2D render target.
			hr = m_pDirect2dFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(m_hwnd, size),
				&m_pRenderTarget
				);


			if (SUCCEEDED(hr))
			{
				for (auto &resources : m_resources)
				{
					hr = resources->Create(m_pDirect2dFactory, m_pRenderTarget);
					if (!SUCCEEDED(hr))
					{
						break;
					}
				}
			}
		}

		return hr;
	}

	void App::DiscardDeviceResources()
	{
		SafeRelease(&m_pRenderTarget);

		for (auto &resources : m_resources)
		{
			resources->Discard();
		}
	}

	LRESULT CALLBACK App::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = 0;

		if (message == WM_CREATE)
		{
			LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
			App *pApp = (App *)pcs->lpCreateParams;

			::SetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA,
				PtrToUlong(pApp)
				);

			result = 1;
		}
		else
		{
			App *pApp = reinterpret_cast<App *>(static_cast<LONG_PTR>(
				::GetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA
				)));

			bool wasHandled = false;

			if (pApp)
			{
				switch (message)
				{
				case WM_MOUSEMOVE:
				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
				{
					INT x = GET_X_LPARAM(lParam);
					INT y = GET_Y_LPARAM(lParam);
					bool needsRefresh = false;
					switch (message)
					{
					case WM_MOUSEMOVE:
						needsRefresh = pApp->m_elementManager->NotifyMouseMove(x, y);
						break;
					case WM_LBUTTONDOWN:
						needsRefresh = pApp->m_elementManager->NotifyMouseDown(x, y);
						break;
					case WM_LBUTTONUP:
						needsRefresh = pApp->m_elementManager->NotifyMouseUp(x, y);
						break;
					}
					if (needsRefresh)
					{
						InvalidateRect(hwnd, NULL, FALSE);
					}
				}
				result = 0;
				wasHandled = true;
				break;
				case WM_SIZE:
				{
					UINT width = LOWORD(lParam);
					UINT height = HIWORD(lParam);
					pApp->OnResize(width, height);
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_DISPLAYCHANGE:
				{
					InvalidateRect(hwnd, NULL, FALSE);
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_PAINT:
				{
					pApp->OnRender();
					ValidateRect(hwnd, NULL);
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_DESTROY:
				{
					PostQuitMessage(0);
				}
				result = 1;
				wasHandled = true;
				break;
				}
			}

			if (!wasHandled)
			{
				result = DefWindowProc(hwnd, message, wParam, lParam);
			}
		}

		return result;
	}



	HRESULT App::OnRender()
	{
		HRESULT hr = S_OK;

		hr = CreateDeviceResources();


		if (SUCCEEDED(hr))
		{
			m_pRenderTarget->BeginDraw();

			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

			m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

			m_elementManager->GetRoot()->ArrangeAndDraw(true);

			hr = m_pRenderTarget->EndDraw();
		}


		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			DiscardDeviceResources();
		}

		return hr;
	}

	void App::OnResize(UINT width, UINT height)
	{
		if (m_pRenderTarget)
		{
			m_pRenderTarget->Resize(D2D1::SizeU(width, height));
		}
	}

}