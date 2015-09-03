#include "include/Common.h"
#include "include/Grid.h"

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

		if (m_cellHeight == 0 || !m_itemsProvider || !m_cellCreateCallback)
		{
			// Invalid state
			return;
		}

		if (m_lastHeightUsedForScrollCheck != GetHeight())
		{
			// Do another scroll height check since the height of the element has changed
			LimitToBounds(m_offsetPercent);
			m_lastHeightUsedForScrollCheck = GetHeight();
		}

		auto totalCount = m_itemsProvider->GetTotalItems();
		auto childrenCount = GetChildrenCount();
		int visibleRows = ceil(GetHeight() / m_cellHeight) + 1; // Need an extra for partial rows
		auto visibleItems = visibleRows * m_columns;
		auto missingChildren = min(totalCount, visibleItems) - childrenCount;

		for (int i = 0; i < missingChildren; i++)
		{
			auto fromThis = dynamic_pointer_cast<Grid>(shared_from_this());
			auto cellContainer = make_shared<Cell>(fromThis, childrenCount + i);
			AddChild(cellContainer);
			cellContainer->AddChild(m_cellCreateCallback());
		}

		// Now do some calculations based on the current parameters
		m_cellWidth = GetWidth() / m_columns;

		auto totalRows = ceil(double(totalCount) / m_columns);
		auto totalContentHeight = (totalRows * m_cellHeight) + m_topPadding + m_bottomPadding;
		auto totalHeightOffset = totalContentHeight * m_offsetPercent;

		int currentRow = floor((totalHeightOffset - m_topPadding) / m_cellHeight);
		m_rowOffset = fmod(totalHeightOffset - m_topPadding, m_cellHeight);

		currentRow = max(0, currentRow);
		m_baseItemIndex = currentRow * m_columns;

	}

	double Grid::GetCurrentOffsetPercent()
	{
		return m_offsetPercent;
	}

	double Grid::GetThumbSizePercent()
	{
		auto totalRows = ceil(double(m_itemsProvider->GetTotalItems()) / m_columns);
		auto totalContentHeight = (totalRows * m_cellHeight) + m_topPadding + m_bottomPadding;
		return min(1.0, GetHeight() / totalContentHeight);
	}

	void Grid::MoveToOffsetPercent(double offsetPercent)
	{
		m_offsetPercent = offsetPercent;
	}

	bool Grid::CanScroll()
	{
		auto totalRows = ceil(double(m_itemsProvider->GetTotalItems()) / m_columns);
		auto totalContentHeight = (totalRows * m_cellHeight) + m_topPadding + m_bottomPadding;
		return totalContentHeight > GetHeight();
	}

	Grid::Cell::Cell(const shared_ptr<Grid>& grid, int index) :
		m_grid(grid), m_index(index)
	{
	}

	void Grid::Cell::PrepareViewModel()
	{
		if (m_grid->m_itemsProvider)
		{
			auto index = m_grid->m_baseItemIndex + m_index;
			if (index < m_grid->m_itemsProvider->GetTotalItems())
			{
				SetViewModel(m_grid->m_itemsProvider->GetItem(index));
			}
			else
			{
				SetViewModel(nullptr);
			}
		}
	}

	void Grid::Cell::Arrange()
	{
		if (GetViewModel() == nullptr)
		{
			SetIsVisible(false);
			return;
		}

		auto row = m_index / m_grid->m_columns;
		auto col = m_index % m_grid->m_columns;

		auto left = (m_grid->GetLeft() + col * m_grid->m_cellWidth); 
		auto right = left + m_grid->m_cellWidth;
		auto top = 
			m_grid->GetTop() - m_grid->m_rowOffset 
			+ row * m_grid->m_cellHeight; 
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

	const double& Grid::GetTopPadding() const
	{
		return m_topPadding;
	}

	void Grid::SetTopPadding(double topPadding)
	{
		m_topPadding = topPadding;
	}

	const double& Grid::GetBottomPadding() const
	{
		return m_bottomPadding;
	}

	void Grid::SetBottomPadding(double bottomPadding)
	{
		m_bottomPadding = bottomPadding;
	}

	const shared_ptr<ItemsProvider>& Grid::GetItemsProvider() const
	{
		return m_itemsProvider;
	}

	void Grid::SetItemsProvider(const shared_ptr<ItemsProvider>& itemsProvider)
	{
		m_itemsProvider = itemsProvider;
	}

	const function<shared_ptr<Element>()>& Grid::GetCellCreateCallback() const
	{
		return m_cellCreateCallback;
	}

	void Grid::SetCellCreateCallback(const function<shared_ptr<Element>()>& cellCreateCallback)
	{
		m_cellCreateCallback = cellCreateCallback;
	}
}