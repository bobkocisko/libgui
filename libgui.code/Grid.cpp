#include "libgui/Common.h"
#include "libgui/Grid.h"

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
    int  visibleRows     = ceil(GetHeight() / _cellHeight) + 1; // Need an extra for partial rows
    auto visibleItems    = visibleRows * _columns;
    auto missingChildren = min(totalCount, visibleItems) - childrenCount;

    for (int i = 0; i < missingChildren; i++)
    {
        auto fromThis      = dynamic_pointer_cast<Grid>(shared_from_this());
        auto cellContainer = make_shared<Cell>(fromThis, childrenCount + i);
        AddChild(cellContainer);
        _cellCreateCallback(cellContainer);
    }

    // Now do some calculations based on the current parameters
    _cellWidth = GetWidth() / _columns;

    auto totalRows          = ceil(double(totalCount) / _columns);
    auto totalContentHeight = (totalRows * _cellHeight) + _topPadding + _bottomPadding;
    auto totalHeightOffset  = totalContentHeight * _offsetPercent;

    int currentRow = floor((totalHeightOffset - _topPadding) / _cellHeight);
    _rowOffset = fmod(totalHeightOffset - _topPadding, _cellHeight);

    currentRow     = max(0, currentRow);
    _baseItemIndex = currentRow * _columns;

}

double Grid::GetCurrentOffsetPercent()
{
    return _offsetPercent;
}

double Grid::GetThumbSizePercent()
{
    auto totalRows          = ceil(double(_itemsProvider->GetTotalItems()) / _columns);
    auto totalContentHeight = (totalRows * _cellHeight) + _topPadding + _bottomPadding;
    return min(1.0, GetHeight() / totalContentHeight);
}

void Grid::MoveToOffsetPercent(double offsetPercent)
{
    _offsetPercent = offsetPercent;
}

bool Grid::CanScroll()
{
    auto totalRows          = ceil(double(_itemsProvider->GetTotalItems()) / _columns);
    auto totalContentHeight = (totalRows * _cellHeight) + _topPadding + _bottomPadding;
    return totalContentHeight > GetHeight();
}

Grid::Cell::Cell(const shared_ptr<Grid>& grid, int index)
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
    if (GetViewModel() == nullptr)
    {
        SetIsVisible(false);
        return;
    }

    auto row = _index / _grid->_columns;
    auto col = _index % _grid->_columns;

    auto left   = (_grid->GetLeft() + col * _grid->_cellWidth);
    auto right  = left + _grid->_cellWidth;
    auto top    =
             _grid->GetTop() - _grid->_rowOffset
             + row * _grid->_cellHeight;
    auto bottom = top + _grid->_cellHeight;

    // Snap to pixel boundaries
    SetLeft(round(left));
    SetRight(round(right));
    SetTop(round(top));
    SetBottom(round(bottom));
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

const shared_ptr<ItemsProvider>& Grid::GetItemsProvider() const
{
    return _itemsProvider;
}

void Grid::SetItemsProvider(const shared_ptr<ItemsProvider>& itemsProvider)
{
    _itemsProvider = itemsProvider;
}

void Grid::SetCellCreateCallback(const function<void(shared_ptr<Element>)>& cellCreateCallback)
{
    _cellCreateCallback = cellCreateCallback;
}
}