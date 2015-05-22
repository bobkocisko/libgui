#include "Precompiled.h"
#include "ItemsViewModel.h"

namespace libgui_sample_windows2
{
	ItemsViewModel::ItemsViewModel()
	{
		items_ = {
			make_shared<ItemViewModel>(L"abc", L"type a", L"123"),
			make_shared<ItemViewModel>(L"abc2", L"type a", L"456"),
			make_shared<ItemViewModel>(L"abc3", L"type a", L"789"),
			make_shared<ItemViewModel>(L"abc4", L"type a", L"123"),
			make_shared<ItemViewModel>(L"abc5", L"type a", L"456"),
			make_shared<ItemViewModel>(L"abc6", L"type a", L"789"),
			make_shared<ItemViewModel>(L"abc7", L"type a", L"123"),
			make_shared<ItemViewModel>(L"abc8", L"type a", L"456"),
			make_shared<ItemViewModel>(L"abc9", L"type a", L"789"),
			make_shared<ItemViewModel>(L"abc10", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"abc11", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"abc12", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"abc13", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"abc14", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"abc15", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"abc16", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"abc17", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"abc18", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"abc", L"type a", L"123"),
			make_shared<ItemViewModel>(L"abc2", L"type a", L"456"),
			make_shared<ItemViewModel>(L"abc3", L"type a", L"789"),
			make_shared<ItemViewModel>(L"abc4", L"type a", L"123"),
			make_shared<ItemViewModel>(L"abc5", L"type a", L"456"),
			make_shared<ItemViewModel>(L"abc6", L"type a", L"789"),
			make_shared<ItemViewModel>(L"abc7", L"type a", L"123"),
			make_shared<ItemViewModel>(L"abc8", L"type a", L"456"),
			make_shared<ItemViewModel>(L"abc9", L"type a", L"789"),
			make_shared<ItemViewModel>(L"abc10", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"abc11", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"abc12", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"abc13", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"abc14", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"abc15", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"abc16", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"abc17", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"abc18", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"abc", L"type a", L"123"),
			make_shared<ItemViewModel>(L"abc2", L"type a", L"456"),
			make_shared<ItemViewModel>(L"abc3", L"type a", L"789"),
			make_shared<ItemViewModel>(L"abc4", L"type a", L"123"),
			make_shared<ItemViewModel>(L"abc5", L"type a", L"456"),
			make_shared<ItemViewModel>(L"abc6", L"type a", L"789"),
			make_shared<ItemViewModel>(L"abc7", L"type a", L"123"),
			make_shared<ItemViewModel>(L"abc8", L"type a", L"456"),
			make_shared<ItemViewModel>(L"abc9", L"type a", L"789"),
			make_shared<ItemViewModel>(L"abc10", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"abc11", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"abc12", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"abc13", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"abc14", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"abc15", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"abc16", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"abc17", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"abc18", L"b is the type", L"789"),
		};
	}

	int ItemsViewModel::GetTotalItems()
	{
		return 15;
	}

	shared_ptr<ItemViewModel> ItemsViewModel::GetItem(int index)
	{
		return items_[index];
	}
}

