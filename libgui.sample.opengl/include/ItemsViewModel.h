#pragma once
#include <libgui/ViewModelBase.h>
#include <libgui/ItemsProvider.h>
#include "ItemViewModel.h"
#include <memory>
#include <vector>


class ItemsViewModel: public libgui::ViewModelBase, public libgui::ItemsProvider
{
public:
  ItemsViewModel();

  int GetTotalItems() override;
  std::shared_ptr<libgui::ViewModelBase> GetItem(int index) override;

  int GetItemIndex(std::shared_ptr<ViewModelBase> item) override;

  void AddOtherItems();

  void RemoveOtherItems();

private:
  std::vector<std::shared_ptr<ItemViewModel>> _items;
};

