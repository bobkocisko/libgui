#include "Precompiled.h"
#include "ItemsViewModel.h"

namespace libgui_sample_windows2
{
	ItemsViewModel::ItemsViewModel()
	{
		m_items = {
			make_shared<ItemViewModel>(L"A-1", L"type a", L"123"),
			make_shared<ItemViewModel>(L"B-1", L"type a", L"456"),
			make_shared<ItemViewModel>(L"C-1", L"type a", L"789"),
			make_shared<ItemViewModel>(L"A-2", L"type a", L"123"),
			make_shared<ItemViewModel>(L"B-2", L"type a", L"456"),
			make_shared<ItemViewModel>(L"C-2", L"type a", L"789"),
			make_shared<ItemViewModel>(L"A-3", L"type a", L"123"),
			make_shared<ItemViewModel>(L"B-3", L"type a", L"456"),
			make_shared<ItemViewModel>(L"C-3", L"type a", L"789"),
			make_shared<ItemViewModel>(L"A-4", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"B-4", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"C-4", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"A-5", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"B-5", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"C-5", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"A-6", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"B-6", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"C-6", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"A-7", L"type a", L"123"),
			make_shared<ItemViewModel>(L"B-7", L"type a", L"456"),
			make_shared<ItemViewModel>(L"C-7", L"type a", L"789"),
			make_shared<ItemViewModel>(L"A-8", L"type a", L"123"),
			make_shared<ItemViewModel>(L"B-8", L"type a", L"456"),
			make_shared<ItemViewModel>(L"C-8", L"type a", L"789"),
			make_shared<ItemViewModel>(L"A-9", L"type a", L"123"),
			make_shared<ItemViewModel>(L"B-9", L"type a", L"456"),
			make_shared<ItemViewModel>(L"C-9", L"type a", L"789"),
			make_shared<ItemViewModel>(L"A-9", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"B-9", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"C-9", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"A-10", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"B-10", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"C-10", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"A-11", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"B-11", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"C-11", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"A-12", L"type a", L"123"),
			make_shared<ItemViewModel>(L"B-12", L"type a", L"456"),
			make_shared<ItemViewModel>(L"C-12", L"type a", L"789"),
			make_shared<ItemViewModel>(L"A-13", L"type a", L"123"),
			make_shared<ItemViewModel>(L"B-13", L"type a", L"456"),
			make_shared<ItemViewModel>(L"C-13", L"type a", L"789"),
			make_shared<ItemViewModel>(L"A-14", L"type a", L"123"),
			make_shared<ItemViewModel>(L"B-14", L"type a", L"456"),
			make_shared<ItemViewModel>(L"C-14", L"type a", L"789"),
			make_shared<ItemViewModel>(L"A-15", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"B-15", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"C-15", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"A-16", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"B-16", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"C-16", L"b is the type", L"789"),
			make_shared<ItemViewModel>(L"A-17", L"b is the type", L"123"),
			make_shared<ItemViewModel>(L"B-17", L"b is the type", L"456"),
			make_shared<ItemViewModel>(L"C-17", L"b is the type", L"789"),
		};
	}

	int ItemsViewModel::GetTotalItems()
	{
		return m_items.size();
	}

	shared_ptr<ViewModelBase> ItemsViewModel::GetItem(int index)
	{
		if (index >= m_items.size())
		{
			return nullptr;
		}
		return m_items[index];
	}
}

