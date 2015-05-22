#include "Precompiled.h"
#include "App.h"
#include "SharedResources.h"
#include "libgui.code\Element.h"
#include "libgui.code\VerticalRepeaterElement.h"
#include "libgui.code\Button.h"
#include "enums.h"
#include "ItemsViewModel.h"
#include <libgui.code/Grid.h>
#include <libgui.code/DrawingManager.h>

namespace libgui_sample_windows2
{
	App::App() :
		m_hwnd(NULL),
		m_pDirect2dFactory(NULL),
		render_target_(NULL)
	{
		m_resources.push_back(SharedResources::Get());

		m_elementManager->SetSystemCaptureCallback([&](bool capture)
		{
			if (capture)
			{
				SetCapture(m_hwnd);
			}
			else
			{
				ReleaseCapture();
			}
		});

		DrawingManager::Get()->SetStartClippingCallback([&](Rect4 r)
		{
			D2D1_RECT_F clipRect;
			clipRect.left = r.left; clipRect.right = r.right;
			clipRect.top = r.top; clipRect.bottom = r.bottom;
			render_target_->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_ALIASED);
		});
		DrawingManager::Get()->SetStopClippingCallback([&]()
		{
			render_target_->PopAxisAlignedClip();
		});

		// Main view model
		auto itemsVm = make_shared<ItemsViewModel>();

		// Set up the root element to match the window size
		auto root = make_shared<Element>();
		m_elementManager->SetRoot(root);
		root->SetElementManager(m_elementManager);
		root->SetViewModel(itemsVm);
		root->SetArrangeCallback([&](shared_ptr<Element> e)
		{
			D2D1_SIZE_F rtSize = render_target_->GetSize();
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

		auto footer = make_shared<Element>();
		{
			root->AddChild(footer);
			footer->SetArrangeCallback([&](shared_ptr<Element> e)
			{
				auto p = e->GetParent();
				e->SetLeft(p->GetLeft()); e->SetRight(p->GetRight());
				e->SetBottom(p->GetBottom()); e->SetHeight(41.0);
			});
			footer->SetDrawCallback([&](shared_ptr<Element> e)
			{
				DrawBorder(1.0, SharedResources::Get()->GrayBrush, e);
			});
		}

		auto grid_scroll_width = 32;

		auto grid = make_shared<Grid>();
		{
			root->AddChild(grid);
			grid->SetArrangeCallback([=](shared_ptr<Element> e)
			{
				auto p = e->GetParent();
				e->SetTop(header->GetBottom()); e->SetBottom(footer->GetTop());

				// CanScroll method depends on the height to be specified first
				auto can_scroll = dynamic_pointer_cast<Grid>(e)->CanScroll();
				if (can_scroll)
				{
					e->SetRight(p->GetRight() - grid_scroll_width - 10);
				}
				else
				{
					e->SetRight(p->GetRight() - 10);
				}
				e->SetLeft(p->GetLeft()); 
			});

			grid->SetTopPadding(5);
			grid->SetBottomPadding(5);

			grid->SetCellHeight(100);
			grid->SetColumns(3);
			grid->SetItemsProvider(itemsVm);

			grid->SetCellCreateCallback([&]()
			{
				auto cell_background = make_shared<Element>();
				cell_background->SetArrangeCallback([](shared_ptr<Element> e)
				{
					auto p = e->GetParent();
					e->SetLeft(p->GetLeft() + 10); e->SetRight(p->GetRight());
					e->SetTop(p->GetTop() + 5); e->SetBottom(p->GetBottom() - 5);
				});

				cell_background->SetDrawCallback([&](shared_ptr<Element> e)
				{
					D2D1_ROUNDED_RECT rr;
					rr.rect.left = e->GetLeft();
					rr.rect.right = e->GetRight();
					rr.rect.top = e->GetTop();
					rr.rect.bottom = e->GetBottom();
					rr.radiusX = 4.5; rr.radiusY = 4.5;

					render_target_->FillRoundedRectangle(rr, SharedResources::Get()->LightGrayBrush);
				});
				{
					auto text = make_shared<Element>();
					cell_background->AddChild(text);
					text->SetDrawCallback([&](shared_ptr<Element> e)
					{
						auto ivm = dynamic_pointer_cast<ItemViewModel>(e->GetViewModel());
						D2D1_RECT_F rect;
						rect.left = e->GetLeft(); rect.right = e->GetRight();
						rect.top = e->GetTop(); rect.bottom = e->GetBottom();
						render_target_->DrawTextW(
							ivm->GetName().c_str(), ivm->GetName().length(),
							m_pTextFormat, rect, SharedResources::Get()->DarkGrayBrush);
					});
				}
				return cell_background;
			});
		}

		auto grid_scroll = make_shared<Scrollbar>(grid);
		{
			root->AddChild(grid_scroll);
			grid_scroll->Init();
			grid_scroll->SetArrangeCallback([=](shared_ptr<Element> e)
			{
				auto p = e->GetParent();
				auto can_scroll = dynamic_pointer_cast<Grid>(grid)->CanScroll();

				if (can_scroll)
				{
					e->SetWidth(grid_scroll_width); e->SetRight(p->GetRight());
					e->SetTop(header->GetBottom()); e->SetBottom(footer->GetTop());
				}
				else
				{
					e->SetIsVisible(false);
				}
			});

			grid_scroll->SetDrawCallback([&](shared_ptr<Element> e)
			{
				// Draw the scrollbar background
				auto sr = SharedResources::Get();
				D2D1_RECT_F rect;
				rect.left = e->GetLeft(); rect.right = e->GetRight();
				rect.top = e->GetTop(); rect.bottom = e->GetBottom();
				render_target_->FillRectangle(rect, sr->LightGrayBrush);
			});

			auto track = grid_scroll->GetTrack();
			track->SetArrangeCallback([=](shared_ptr<Element> e)
			{
				auto p = e->GetParent();
				e->SetLeft(p->GetLeft() + 5); e->SetRight(p->GetRight() - 5);
				e->SetTop(p->GetTop() + 5); e->SetBottom(p->GetBottom() - 5);
			});

			auto thumb = grid_scroll->GetThumb();
			thumb->SetDrawCallback([&](shared_ptr<Element> e)
			{
				auto sr = SharedResources::Get();
				D2D1_RECT_F rect;
				rect.left = e->GetLeft(); rect.right = e->GetRight();
				rect.top = e->GetTop(); rect.bottom = e->GetBottom();
				render_target_->FillRectangle(rect, sr->MediumGrayBrush);
			});


		}
	}

	void App::DrawBorder(double strokeWidth, ID2D1Brush* brush, shared_ptr<Element> e)
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
			render_target_->DrawRectangle(rect, brush, strokeWidth);
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
				render_target_->DrawLine(
					D2D1::Point2F(rect.left, outerRect.top),
					D2D1::Point2F(rect.left, outerRect.bottom),
					brush, strokeWidth);
			}
			if (0 < (sides & RIGHT))
			{
				render_target_->DrawLine(
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
				render_target_->DrawLine(
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
				render_target_->DrawLine(
					D2D1::Point2F(left, rect.bottom),
					D2D1::Point2F(right, rect.bottom),
					brush, strokeWidth);
			}
		}
	}

	App::~App()
	{
		SafeRelease(&m_pDirect2dFactory);

		SafeRelease(&m_pTextFormat);
		SafeRelease(&m_pTextFormat);

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
				static_cast<UINT>(ceil(700.f * dpiX / 96.f)),
				static_cast<UINT>(ceil(400.f * dpiY / 96.f)),
				NULL,
				NULL,
				HINST_THISCOMPONENT,
				this
				);
			hr = m_hwnd ? S_OK : E_FAIL;
			if (SUCCEEDED(hr))
			{
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

		if (SUCCEEDED(hr))
		{
			hr = DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(m_pDWriteFactory),
				reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
				);
		}
		if (SUCCEEDED(hr))
		{
			// Create a DirectWrite text format object.
			hr = m_pDWriteFactory->CreateTextFormat(
				L"Segoe Script",
				NULL,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				14.0,
				L"", //locale
				&m_pTextFormat
				);
		}
		if (SUCCEEDED(hr))
		{
			// Center the text horizontally and vertically.
			m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

			m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}

		return hr;
	}

	HRESULT App::CreateDeviceResources()
	{
		HRESULT hr = S_OK;

		if (!render_target_)
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
				&render_target_
				);


			if (SUCCEEDED(hr))
			{
				for (auto &resources : m_resources)
				{
					hr = resources->Create(m_pDirect2dFactory, render_target_);
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
		SafeRelease(&render_target_);

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
			render_target_->BeginDraw();

			render_target_->SetTransform(D2D1::Matrix3x2F::Identity());

			render_target_->Clear(D2D1::ColorF(D2D1::ColorF::White));

			m_elementManager->GetRoot()->ArrangeAndDraw(true);

			hr = render_target_->EndDraw();
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
		if (render_target_)
		{
			render_target_->Resize(D2D1::SizeU(width, height));
		}
	}

}