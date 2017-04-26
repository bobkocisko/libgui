#include "include/ItemViewModel.h"

ItemViewModel::ItemViewModel(const std::string& name, const std::string& type, const std::string& size)
  :
  name_(name), type_(type), size_(size)
{
}

const std::string& ItemViewModel::GetName() const
{
  return name_;
}

void ItemViewModel::SetName(const std::string& name)
{
  name_ = name;
}

const std::string& ItemViewModel::GetType() const
{
  return type_;
}

void ItemViewModel::SetType(const std::string& type)
{
  type_ = type;
}

const std::string& ItemViewModel::GetSize() const
{
  return size_;
}

void ItemViewModel::SetSize(const std::string& size)
{
  size_ = size;
}