#pragma once
#include <libgui.code/ViewModelBase.h>

using namespace std;

namespace libgui_sample_windows2
{
	class ItemViewModel: public libgui::ViewModelBase
	{
	public:
		ItemViewModel(const wstring& name, const wstring& type, const wstring& size);

		const wstring& GetName() const;
		void SetName(const wstring& name);

		const wstring& GetType() const;
		void SetType(const wstring& type);

		const wstring& GetSize() const;
		void SetSize(const wstring& size);

	private:
		wstring name_;
		wstring type_;
		wstring size_;
	};
}

