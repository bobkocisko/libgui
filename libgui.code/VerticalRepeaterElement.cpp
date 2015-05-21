#include "Precompiled.h"
#include "VerticalRepeaterElement.h"

namespace libgui
{
	void VerticalRepeaterElement::Arrange()
	{
		// First perform arrangement for the repeater element itself
		Element::Arrange();

		// Then create children if necessary

		if (row_height_ == 0 || !total_count_callback_ || !row_create_callback_)
		{
			// Invalid state
			return;
		}

		auto currentChildrenCount = GetChildrenCount();
		auto totalCount = total_count_callback_(shared_from_this());
		auto missingChildren = totalCount - currentChildrenCount;

		for (int i = 0; i < missingChildren; i++)
		{
			auto rowContainer = make_shared<RowElement>(
				dynamic_pointer_cast<VerticalRepeaterElement>(shared_from_this()),
				currentChildrenCount + i);
			AddChild(rowContainer);
			rowContainer->AddChild(row_create_callback_());
		}

		// TODO: support removing children to match viewmodel
	}

	VerticalRepeaterElement::RowElement::RowElement(shared_ptr<VerticalRepeaterElement> parent, int index)
		:parent_(parent), index_(index)
	{ }

	void VerticalRepeaterElement::RowElement::Arrange()
	{
		// Get the view model for this particular row's index
		if (parent_->row_view_model_callback_)
		{
			SetViewModel(parent_->row_view_model_callback_(parent_->GetViewModel(), index_));
		}

		auto ps = GetPrevSibling();

		SetLeft(parent_->GetLeft());
		SetRight(parent_->GetRight());

		SetHeight(parent_->row_height_);
		if (ps)
		{
			SetTop(ps->GetBottom());
		}
		else
		{
			SetTop(parent_->GetTop());
		}
	}

	void VerticalRepeaterElement::SetRowHeight(double rowHeight)
	{
		row_height_ = rowHeight;
	}
	double VerticalRepeaterElement::GetRowHeight()
	{
		return row_height_;
	}

	void VerticalRepeaterElement::SetRowCreateCallback(function<shared_ptr<Element>()> rowCreateCallback)
	{
		row_create_callback_ = rowCreateCallback;
	}
	void VerticalRepeaterElement::SetTotalCountCallback(function<int(shared_ptr<Element>)> totalCountCallback)
	{
		total_count_callback_ = totalCountCallback;
	}
	void VerticalRepeaterElement::SetRowViewModelCallback(function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)> rowViewModelCallback)
	{
		row_view_model_callback_ = rowViewModelCallback;
	}

}