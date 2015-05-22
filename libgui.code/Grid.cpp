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

		if (m_cellHeight == 0 || !m_totalCountCallback || !m_cellCreateCallback)
		{
			// Invalid state
			return;
		}

		auto fromThis = dynamic_pointer_cast<Grid>(shared_from_this());

		auto totalCount = m_totalCountCallback(fromThis);
		auto childrenCount = GetChildrenCount();
		int visibleRows = ceil(GetHeight() / m_cellHeight);
		auto visibleItems = visibleRows * m_columns;
		auto missingChildren = min(totalCount, visibleItems) - childrenCount;

		for (int i = 0; i < missingChildren; i++)
		{
			auto cell_container = make_shared<Cell>(fromThis, childrenCount + i);
			AddChild(cell_container);
			cell_container->AddChild(m_cellCreateCallback());
		}

		// Now determine the cell width based on the current parameters
		m_cellWidth = GetWidth() / m_columns;
	}

	double Grid::GetCurrentOffsetPercent()
	{
		return m_offsetPercent;
	}

	double Grid::GetThumbSizePercent()
	{
		auto totalRows = ceil(m_totalCountCallback(shared_from_this()) / m_columns);
		return min(1.0, GetHeight() / (totalRows * m_cellHeight));
	}

	void Grid::MoveToOffsetPercent(double offsetPercent)
	{
		m_offsetPercent = offsetPercent;
	}

	bool Grid::CanScroll()
	{
		auto totalRows = ceil(m_totalCountCallback(shared_from_this()) / m_columns);
		return (totalRows * m_cellHeight) > GetHeight();
	}

	Grid::Cell::Cell(const shared_ptr<Grid>& grid, int index) :
		m_grid(grid), m_index(index)
	{
	}

	void Grid::Cell::PrepareViewModel()
	{
		if (m_grid->m_cellViewModelCallback)
		{
			SetViewModel(m_grid->m_cellViewModelCallback(m_grid->GetViewModel(), m_index));
		}
	}

	void Grid::Cell::Arrange()
	{
		auto row = m_index / m_grid->m_columns;
		auto col = m_index % m_grid->m_columns;

		auto left = (m_grid->GetLeft() + col * m_grid->m_cellWidth); 
		auto right = left + m_grid->m_cellWidth;
		auto top = (m_grid->GetTop() + row * m_grid->m_cellHeight); 
		auto bottom = top + m_grid->m_cellHeight;

		// Snap to pixel boundaries
		SetLeft(round(left)); SetRight(round(right));
		SetTop(round(top)); SetBottom(round(bottom));
	}


	const int& Grid::GetColumns() const
	{
		return m_columns;
	}

	void Grid::SetColumns(int columns)
	{
		m_columns = columns;
	}

	void Grid::SetCellHeight(double cellHeight)
	{
		m_cellHeight = cellHeight;
	}

	double Grid::GetCellHeight()
	{
		return m_cellHeight;
	}

	const function<shared_ptr<Element>()>& Grid::GetCellCreateCallback() const
	{
		return m_cellCreateCallback;
	}

	void Grid::SetCellCreateCallback(const function<shared_ptr<Element>()>& cellCreateCallback)
	{
		m_cellCreateCallback = cellCreateCallback;
	}

	const function<int(shared_ptr<Element>)>& Grid::GetTotalCountCallback() const
	{
		return m_totalCountCallback;
	}

	void Grid::SetTotalCountCallback(const function<int(shared_ptr<Element>)>& totalCountCallback)
	{
		m_totalCountCallback = totalCountCallback;
	}

	const function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)>& Grid::GetCellViewModelCallback() const
	{
		return m_cellViewModelCallback;
	}

	void Grid::SetCellViewModelCallback(const function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)>& cellViewModelCallback)
	{
		m_cellViewModelCallback = cellViewModelCallback;
	}

	const shared_ptr<Scrollbar>& Grid::GetScrollbar() const
	{
		return m_scrollbar;
	}

	void Grid::SetScrollbar(const shared_ptr<Scrollbar>& scrollbar)
	{
		m_scrollbar = scrollbar;
	}

}