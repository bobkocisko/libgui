#pragma once

#include "/libgui.code/include/ViewModelBase.h"

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

		bool GetIsChecked();

		void ToggleIsChecked();

	private:
		std::wstring m_text;
		bool m_isChecked = false;
	};

	class AddHintRowViewModel : public RowViewModel
	{

	};

	class EmptyRowViewModel : public RowViewModel
	{

	};
}