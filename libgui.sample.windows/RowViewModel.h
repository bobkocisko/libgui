#pragma once

#include "libgui.code\ViewModelBase.h"

using namespace std;
using namespace libgui;

namespace libgui_sample_windows
{
	class RowViewModel : public ViewModelBase
	{
	protected:
		RowViewModel();

	};

	class TodoRowViewModel : public RowViewModel
	{
	public:
		TodoRowViewModel();
		TodoRowViewModel(wstring text);

		wstring GetText();

	private:
		std::wstring m_text;
	};

	class AddHintRowViewModel : public RowViewModel
	{

	};

	class EmptyRowViewModel : public RowViewModel
	{

	};
}