#pragma once
#include </libgui.code/include/ViewModelBase.h>
#include </libgui.code/include/ItemsProvider.h>
#include "ItemViewModel.h"

using namespace std;
using namespace libgui;

namespace libgui_sample_windows2
{
	class ItemsViewModel : public ViewModelBase, public ItemsProvider
	{
	public:
		ItemsViewModel();

		int GetTotalItems() override;
		shared_ptr<ViewModelBase> GetItem(int index) override;

	private:
		vector<shared_ptr<ItemViewModel>> m_items;
	};
}

