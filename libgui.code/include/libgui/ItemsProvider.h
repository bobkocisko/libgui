#pragma once

#include <memory>

namespace libgui
{
class ViewModelBase;

class ItemsProvider
{
public:
  virtual ~ItemsProvider();

  virtual int GetTotalItems()    = 0;
  virtual std::shared_ptr<ViewModelBase> GetItem(int index) = 0;
};
}

