#include "Precompiled.h"

#include "RowViewModel.h"

namespace libgui_sample_windows
{
	RowViewModel::RowViewModel()
	{ }

	TodoRowViewModel::TodoRowViewModel(wstring text)
	{
		m_text = text;
	}

	wstring TodoRowViewModel::GetText()
	{
		return m_text;
	}
}