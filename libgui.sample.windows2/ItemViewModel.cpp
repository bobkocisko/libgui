#include "include/Common.h"
#include "include/ItemViewModel.h"

libgui_sample_windows2::ItemViewModel::ItemViewModel(const wstring& name, const wstring& type, const wstring& size) :
name_(name), type_(type), size_(size)
{
}

const wstring& libgui_sample_windows2::ItemViewModel::GetName() const
{
	return name_;
}

void libgui_sample_windows2::ItemViewModel::SetName(const wstring& name)
{
	name_ = name;
}

const wstring& libgui_sample_windows2::ItemViewModel::GetType() const
{
	return type_;
}

void libgui_sample_windows2::ItemViewModel::SetType(const wstring& type)
{
	type_ = type;
}

const wstring& libgui_sample_windows2::ItemViewModel::GetSize() const
{
	return size_;
}

void libgui_sample_windows2::ItemViewModel::SetSize(const wstring& size)
{
	size_ = size;
}