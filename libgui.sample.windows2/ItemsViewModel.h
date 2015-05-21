#pragma once
#include <libgui.code/ViewModelBase.h>
#include "ItemViewModel.h"

using namespace std;

namespace libgui_sample_windows2
{
	class ItemsViewModel : public libgui::ViewModelBase
	{
	public:
		ItemsViewModel();

		int GetTotalItems();
		shared_ptr<ItemViewModel> GetItem(int index);

	private:
		vector<shared_ptr<ItemViewModel>> items_;
	};
}

