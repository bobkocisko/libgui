#pragma once

#include "RowViewModel.h"

using namespace std;

namespace libgui_sample_windows
{
	class PageViewModel : public ViewModelBase
	{
	public:
		PageViewModel();

		vector<shared_ptr<RowViewModel>> GetRows();
		shared_ptr<RowViewModel> GetRow(int);

		int GetTotalRows();

	private:
		vector<shared_ptr<RowViewModel>> m_rows;
	};
}