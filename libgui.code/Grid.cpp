#include "Precompiled.h"
#include "Grid.h"

namespace libgui
{
	Grid::Grid()
	{
		SetClipToBounds(true);
	}

	void Grid::Arrange()
	{
		// First arrange the element itself
		Element::Arrange();

		// Then create children if needed

		if (cell_height_ == 0 || !total_count_callback_ || !cell_create_callback_)
		{
			// Invalid state
			return;
		}

		auto from_this = dynamic_pointer_cast<Grid>(shared_from_this());

		auto total_count = total_count_callback_(from_this);
		auto children_count = GetChildrenCount();
		int visible_rows = ceil(GetHeight() / cell_height_);
		auto visible_items = visible_rows * columns_;
		auto missing_children = min(total_count, visible_items) - children_count;

		for (int i = 0; i < missing_children; i++)
		{
			auto cell_container = make_shared<Cell>(from_this, children_count + i);
			AddChild(cell_container);
			cell_container->AddChild(cell_create_callback_());
		}

		// Now determine the cell width based on the current parameters
		cell_width_ = GetWidth() / columns_;
	}

	double Grid::GetCurrentOffsetPercent()
	{
		return offset_percent_;
	}

	double Grid::GetThumbSizePercent()
	{
		auto total_rows = ceil(total_count_callback_(shared_from_this()) / columns_);
		return min(1.0, GetHeight() / (total_rows * cell_height_));
	}

	void Grid::MoveToOffsetPercent(double offset_percent)
	{
		offset_percent_ = offset_percent;
	}

	bool Grid::CanScroll()
	{
		auto total_rows = ceil(total_count_callback_(shared_from_this()) / columns_);
		return (total_rows * cell_height_) > GetHeight();
	}

	Grid::Cell::Cell(const shared_ptr<Grid>& grid, int index) :
		grid_(grid), index_(index)
	{
	}

	void Grid::Cell::PrepareViewModel()
	{
		if (grid_->cell_view_model_callback_)
		{
			SetViewModel(grid_->cell_view_model_callback_(grid_->GetViewModel(), index_));
		}
	}

	void Grid::Cell::Arrange()
	{
		auto p = GetParent();

		auto row = index_ / grid_->columns_;
		auto col = index_ % grid_->columns_;

		auto left = (p->GetLeft() + col * grid_->cell_width_); 
		auto right = left + grid_->cell_width_;
		auto top = (p->GetTop() + row * grid_->cell_height_); 
		auto bottom = top + grid_->cell_height_;

		// Snap to pixel boundaries
		SetLeft(round(left)); SetRight(round(right));
		SetTop(round(top)); SetBottom(round(bottom));
	}


	const int& Grid::GetColumns() const
	{
		return columns_;
	}

	void Grid::SetColumns(int columns)
	{
		columns_ = columns;
	}

	void Grid::SetCellHeight(double cell_height)
	{
		cell_height_ = cell_height;
	}

	double Grid::GetCellHeight()
	{
		return cell_height_;
	}

	const function<shared_ptr<Element>()>& Grid::GetCellCreateCallback() const
	{
		return cell_create_callback_;
	}

	void Grid::SetCellCreateCallback(const function<shared_ptr<Element>()>& cell_create_callback)
	{
		cell_create_callback_ = cell_create_callback;
	}

	const function<int(shared_ptr<Element>)>& Grid::GetTotalCountCallback() const
	{
		return total_count_callback_;
	}

	void Grid::SetTotalCountCallback(const function<int(shared_ptr<Element>)>& total_count_callback)
	{
		total_count_callback_ = total_count_callback;
	}

	const function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)>& Grid::GetCellViewModelCallback() const
	{
		return cell_view_model_callback_;
	}

	void Grid::SetCellViewModelCallback(const function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)>& cell_view_model_callback)
	{
		cell_view_model_callback_ = cell_view_model_callback;
	}

	const shared_ptr<Scrollbar>& Grid::GetScrollbar() const
	{
		return scrollbar_;
	}

	void Grid::SetScrollbar(const shared_ptr<Scrollbar>& scrollbar)
	{
		scrollbar_ = scrollbar;
	}

}