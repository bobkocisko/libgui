#include "include/ItemsViewModel.h"

using std::make_shared;

ItemsViewModel::ItemsViewModel()
{
  _items = {
    make_shared<ItemViewModel>("A-1", "type a", "123"),
    make_shared<ItemViewModel>("B-1", "type a", "456"),
    make_shared<ItemViewModel>("C-1", "type a", "789"),
    make_shared<ItemViewModel>("A-2", "type a", "123"),
    make_shared<ItemViewModel>("B-2", "type a", "456"),
    make_shared<ItemViewModel>("C-2", "type a", "789"),
    make_shared<ItemViewModel>("A-3", "type a", "123"),
    make_shared<ItemViewModel>("B-3", "type a", "456"),
    make_shared<ItemViewModel>("C-3", "type a", "789"),
    make_shared<ItemViewModel>("A-4", "b is the type", "123"),
    make_shared<ItemViewModel>("B-4", "b is the type", "456"),
    make_shared<ItemViewModel>("C-4", "b is the type", "789"),
    make_shared<ItemViewModel>("A-5", "b is the type", "123"),
    make_shared<ItemViewModel>("B-5", "b is the type", "456"),
    make_shared<ItemViewModel>("C-5", "b is the type", "789"),
    make_shared<ItemViewModel>("A-6", "b is the type", "123"),
    make_shared<ItemViewModel>("B-6", "b is the type", "456"),
    make_shared<ItemViewModel>("C-6", "b is the type", "789"),
    make_shared<ItemViewModel>("A-7", "type a", "123"),
    make_shared<ItemViewModel>("B-7", "type a", "456"),
    make_shared<ItemViewModel>("C-7", "type a", "789"),
    make_shared<ItemViewModel>("A-8", "type a", "123"),
    make_shared<ItemViewModel>("B-8", "type a", "456"),
    make_shared<ItemViewModel>("C-8", "type a", "789"),
    make_shared<ItemViewModel>("A-9", "type a", "123"),
    make_shared<ItemViewModel>("B-9", "type a", "456"),
    make_shared<ItemViewModel>("C-9", "type a", "789"),
    make_shared<ItemViewModel>("A-9", "b is the type", "123"),
    make_shared<ItemViewModel>("B-9", "b is the type", "456"),
    make_shared<ItemViewModel>("C-9", "b is the type", "789"),
    make_shared<ItemViewModel>("A-10", "b is the type", "123"),
    make_shared<ItemViewModel>("B-10", "b is the type", "456"),
    make_shared<ItemViewModel>("C-10", "b is the type", "789"),
    make_shared<ItemViewModel>("A-11", "b is the type", "123"),
    make_shared<ItemViewModel>("B-11", "b is the type", "456"),
    make_shared<ItemViewModel>("C-11", "b is the type", "789"),
    make_shared<ItemViewModel>("A-12", "type a", "123"),
    make_shared<ItemViewModel>("B-12", "type a", "456"),
    make_shared<ItemViewModel>("C-12", "type a", "789"),
    make_shared<ItemViewModel>("A-13", "type a", "123"),
    make_shared<ItemViewModel>("B-13", "type a", "456"),
    make_shared<ItemViewModel>("C-13", "type a", "789"),
    make_shared<ItemViewModel>("A-14", "type a", "123"),
    make_shared<ItemViewModel>("B-14", "type a", "456"),
    make_shared<ItemViewModel>("C-14", "type a", "789"),
    make_shared<ItemViewModel>("A-15", "b is the type", "123"),
    make_shared<ItemViewModel>("B-15", "b is the type", "456"),
    make_shared<ItemViewModel>("C-15", "b is the type", "789"),
    make_shared<ItemViewModel>("A-16", "b is the type", "123"),
    make_shared<ItemViewModel>("B-16", "b is the type", "456"),
    make_shared<ItemViewModel>("C-16", "b is the type", "789"),
    make_shared<ItemViewModel>("A-17", "b is the type", "123"),
    make_shared<ItemViewModel>("B-17", "b is the type", "456"),
    make_shared<ItemViewModel>("C-17", "b is the type", "789"),
  };
}

int ItemsViewModel::GetTotalItems()
{
  return _items.size();
}

std::shared_ptr<libgui::ViewModelBase> ItemsViewModel::GetItem(int index)
{
  if (index >= _items.size())
  {
    return nullptr;
  }
  return _items[index];
}


