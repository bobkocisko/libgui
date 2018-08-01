#include "libgui/Grid.h"

#include <cmath>

namespace libgui
{
Grid::Grid(Element::Dependencies elementDependencies)
  : Element(elementDependencies, "Grid")
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

  auto totalCount = _itemsProvider->GetTotalItems();

  if (_lastHeightUsedForScrollCheck != GetHeight() ||
      _lastItemCountUsedForScrollCheck != totalCount)
  {
    // Do another scroll height check since the height of the element has changed
    LimitToBounds(_offsetPercent);

    // Notify that the thumb size should be recalculated
    if (_thumbDataChangeCallback)
    {
      _thumbDataChangeCallback();
    }

    _lastHeightUsedForScrollCheck = GetHeight();
    _lastItemCountUsedForScrollCheck = totalCount;
  }

  auto childrenCount   = GetChildrenCount();
  int  visibleRows     = int(std::ceil(GetHeight() / _cellHeight)) + 1; // Need an extra for partial rows
  auto visibleItems    = visibleRows * _columns;
  auto missingChildren = std::min(totalCount, visibleItems) - childrenCount;

  for (int i = 0; i < missingChildren; i++)
  {
    auto cellContainer = this->CreateChild<Cell>(childrenCount + i);
    _cellCreateCallback(cellContainer);
    cellContainer->UpdateAfterAdd();
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

void Grid::WhenThumbDataChanges(const std::function<void()>& handler)
{
  _thumbDataChangeCallback = handler;
}

void Grid::MoveToOffsetPercent(double offsetPercent, bool notify_thumb)
{
  _offsetPercent = offsetPercent;

  if (notify_thumb)
  {
    if (_thumbDataChangeCallback)
    {
      _thumbDataChangeCallback();
    }
  }
}

void Grid::ScrollToTop()
{
  MoveToOffsetPercent(0.0, true);
}

void Grid::ScrollTo(std::shared_ptr<ViewModelBase> item)
{
  if (!CanScroll()) return; // All items are visible

  auto index = _itemsProvider->GetItemIndex(item);
  if (-1 == index) return;

  // Figure out what row this item is in
  int row = index / _columns;

  auto totalRows          = std::ceil(double(_itemsProvider->GetTotalItems()) / _columns);
  auto totalContentHeight = (totalRows * _cellHeight) + _topPadding + _bottomPadding;

  auto rowTop = (row * _cellHeight) + _topPadding;

  // Move so that the top of the row is at the top of the viewport
  double offsetPercent = (rowTop / totalContentHeight);

  // But make sure that we don't go beyond the scroll bounds
  LimitToBounds(offsetPercent);

  MoveToOffsetPercent(offsetPercent, true);
}

bool Grid::CanScroll()
{
  if (!_itemsProvider) return false; // No content

  auto totalRows          = std::ceil(double(_itemsProvider->GetTotalItems()) / _columns);
  auto totalContentHeight = (totalRows * _cellHeight) + _topPadding + _bottomPadding;
  return totalContentHeight > GetHeight();
}

Grid::Cell::Cell(Element::Dependencies elementDependencies, int index)
  : Element(elementDependencies, "Grid::Cell"),
    _grid(std::dynamic_pointer_cast<Grid>(elementDependencies.parent)),
    _index(index)
{
}

void Grid::Cell::PrepareViewModel()
{
  if (auto grid = _grid.lock())
  {
    if (grid->_itemsProvider)
    {
      auto index = grid->_baseItemIndex + _index;
      if (index < grid->_itemsProvider->GetTotalItems())
      {
        SetViewModel(grid->_itemsProvider->GetItem(index));
      }
      else
      {
        SetViewModel(nullptr);
      }
    }
  }
}

void Grid::Cell::Arrange()
{
  if (auto grid = _grid.lock())
  {
    SetIsVisible(GetViewModel() != nullptr);

    auto row = _index / grid->_columns;
    auto col = _index % grid->_columns;

    auto left   = (grid->GetLeft() + col * grid->_cellWidth);
    auto right  = left + grid->_cellWidth;
    auto top    = grid->GetTop() - grid->_rowOffset
                  + row * grid->_cellHeight;
    auto bottom = top + grid->_cellHeight;

    // Snap to pixel boundaries
    SetLeft(std::round(left));
    SetRight(std::round(right));
    SetTop(std::round(top));
    SetBottom(std::round(bottom));
  }
}

int Grid::GetColumns() const
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

double Grid::GetTopPadding() const
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

std::shared_ptr<ItemsProvider> Grid::GetItemsProvider() const
{
  return _itemsProvider;
}

void Grid::SetItemsProvider(std::shared_ptr<ItemsProvider> itemsProvider)
{
  _itemsProvider = itemsProvider;
}

void Grid::SetCellCreateCallback(const std::function<void(std::shared_ptr<Element>)>& cellCreateCallback)
{
  _cellCreateCallback = cellCreateCallback;
}

void Grid::OnElementIsBeingRemoved()
{
  // Release anything held in a lambda capture
  _cellCreateCallback = nullptr;
}

}