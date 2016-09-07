#include "libgui/Grid.h"

#include <cmath>

namespace libgui
{
Grid::Grid()
{
    SetClipToBounds(true);
    SetUpdateRearrangesDescendants(true);
}

void Grid::Arrange()
{
    // First arrange the element itself
    Element::Arrange();

    // Then create children if needed

    if (_cellHeight == 0 || !_itemsProvider || !_cellCreateCallback)
    {
        // Invalid state
        return;
    }

    if (_lastHeightUsedForScrollCheck != GetHeight())
    {
        // Do another scroll height check since the height of the element has changed
        LimitToBounds(_offsetPercent);
        _lastHeightUsedForScrollCheck = GetHeight();
    }

    auto totalCount      = _itemsProvider->GetTotalItems();
    auto childrenCount   = GetChildrenCount();
    int  visibleRows     = int(std::ceil(GetHeight() / _cellHeight)) + 1; // Need an extra for partial rows
    auto visibleItems    = visibleRows * _columns;
    auto missingChildren = std::min(totalCount, visibleItems) - childrenCount;

    for (int i = 0; i < missingChildren; i++)
    {
        auto fromThis      = std::dynamic_pointer_cast<Grid>(shared_from_this());
        auto cellContainer = std::make_shared<Cell>(fromThis, childrenCount + i);
        AddChild(cellContainer);
        _cellCreateCallback(cellContainer);
    }

    // Now do some calculations based on the current parameters
    _cellWidth = GetWidth() / _columns;

    auto totalRows          = std::ceil(double(totalCount) / _columns);
    auto totalContentHeight = (totalRows * _cellHeight) + _topPadding + _bottomPadding;
    auto totalHeightOffset  = totalContentHeight * _offsetPercent;

    int currentRow = int(std::floor((totalHeightOffset - _topPadding) / _cellHeight));
    _rowOffset = fmod(totalHeightOffset - _topPadding, _cellHeight);

    currentRow     = std::max(0, currentRow);
    _baseItemIndex = currentRow * _columns;

}

double Grid::GetCurrentOffsetPercent()
{
    return _offsetPercent;
}

double Grid::GetThumbSizePercent()
{
    auto totalRows          = std::ceil(double(_itemsProvider->GetTotalItems()) / _columns);
    auto totalContentHeight = (totalRows * _cellHeight) + _topPadding + _bottomPadding;
    return std::min(1.0, GetHeight() / totalContentHeight);
}

void Grid::MoveToOffsetPercent(double offsetPercent)
{
    _offsetPercent = offsetPercent;
}

bool Grid::CanScroll()
{
    auto totalRows          = std::ceil(double(_itemsProvider->GetTotalItems()) / _columns);
    auto totalContentHeight = (totalRows * _cellHeight) + _topPadding + _bottomPadding;
    return totalContentHeight > GetHeight();
}

Grid::Cell::Cell(const std::shared_ptr<Grid>& grid, int index)
    :
    _grid(grid), _index(index)
{
}

void Grid::Cell::PrepareViewModel()
{
    if (_grid->_itemsProvider)
    {
        auto index = _grid->_baseItemIndex + _index;
        if (index < _grid->_itemsProvider->GetTotalItems())
        {
            SetViewModel(_grid->_itemsProvider->GetItem(index));
        }
        else
        {
            SetViewModel(nullptr);
        }
    }
}

void Grid::Cell::Arrange()
{
    SetIsVisible(GetViewModel() != nullptr);

    auto row = _index / _grid->_columns;
    auto col = _index % _grid->_columns;

    auto left   = (_grid->GetLeft() + col * _grid->_cellWidth);
    auto right  = left + _grid->_cellWidth;
    auto top    = _grid->GetTop() - _grid->_rowOffset
                  + row * _grid->_cellHeight;
    auto bottom = top + _grid->_cellHeight;

    // Snap to pixel boundaries
    SetLeft(std::round(left));
    SetRight(std::round(right));
    SetTop(std::round(top));
    SetBottom(std::round(bottom));
}

const int& Grid::GetColumns() const
{
    return _columns;
}

void Grid::SetColumns(int columns)
{
    _columns = columns;
}

void Grid::SetCellHeight(double cellHeight)
{
    _cellHeight = cellHeight;
}

double Grid::GetCellHeight()
{
    return _cellHeight;
}

const double& Grid::GetTopPadding() const
{
    return _topPadding;
}

void Grid::SetTopPadding(double topPadding)
{
    _topPadding = topPadding;
}

const double& Grid::GetBottomPadding() const
{
    return _bottomPadding;
}

void Grid::SetBottomPadding(double bottomPadding)
{
    _bottomPadding = bottomPadding;
}

const std::shared_ptr<ItemsProvider>& Grid::GetItemsProvider() const
{
    return _itemsProvider;
}

void Grid::SetItemsProvider(const std::shared_ptr<ItemsProvider>& itemsProvider)
{
    _itemsProvider = itemsProvider;
}

void Grid::SetCellCreateCallback(const std::function<void(std::shared_ptr<Element>)>& cellCreateCallback)
{
    _cellCreateCallback = cellCreateCallback;
}

std::string Grid::GetTypeName()
{
    return "Grid";
}

std::string Grid::Cell::GetTypeName()
{
    return "Cell";
}
}