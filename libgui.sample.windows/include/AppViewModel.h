#pragma once

#include "PageViewModel.h"

namespace libgui_sample_windows
{
	class AppViewModel : public ViewModelBase
	{
	public:
		AppViewModel();

		vector<shared_ptr<PageViewModel>> GetPages();

		shared_ptr<PageViewModel> GetCurrentPage();

	private:
		int m_currentPageIndex = 0;
		vector<shared_ptr<PageViewModel>> m_pages;
	};
}