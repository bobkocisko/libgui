#include "Precompiled.h"
#include "AppViewModel.h"

namespace libgui_sample_windows
{
	AppViewModel::AppViewModel()
	{
		m_pages.push_back(make_shared<PageViewModel>());
	}

	vector<shared_ptr<PageViewModel>> AppViewModel::GetPages()
	{
		return m_pages;
	}

	shared_ptr<PageViewModel> AppViewModel::GetCurrentPage()
	{
		return m_pages[m_currentPageIndex];
	}
}