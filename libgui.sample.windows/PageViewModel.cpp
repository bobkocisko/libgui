#include "include/Common.h"

#include "include/PageViewModel.h"

namespace libgui_sample_windows
{
	PageViewModel::PageViewModel()
	{
		m_rows.push_back(make_shared<TodoRowViewModel>(L"Take out the trash"));
		m_rows.push_back(make_shared<TodoRowViewModel>(L"Do laundry"));
		m_rows.push_back(make_shared<TodoRowViewModel>(L"Feed the cat"));
		m_rows.push_back(make_shared<AddHintRowViewModel>());
		m_rows.push_back(make_shared<EmptyRowViewModel>());
	}

	vector<shared_ptr<RowViewModel>> PageViewModel::GetRows()
	{
		return m_rows;
	}

	int PageViewModel::GetTotalRows()
	{
		return m_rows.size();
	}

	shared_ptr<RowViewModel> PageViewModel::GetRow(int index)
	{
		return m_rows[index];
	}
}