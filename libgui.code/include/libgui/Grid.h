#pragma once
#include "Element.h"
#include "Scrollbar.h"
#include "ItemsProvider.h"

namespace libgui
{
class Grid: public Element, public ScrollDelegate
{

public:
  Grid(Element::Dependencies elementDependencies);

  void Arrange() override;

  double GetCurrentOffsetPercent() override;
  double GetThumbSizePercent() override;

  void WhenThumbDataChanges(
    const std::function<void()>& handler) override;

  void MoveToOffsetPercent(double offsetPercent, bool notify_thumb = true) override;

  bool CanScroll();

  int GetColumns() const;
  void SetColumns(int columns);

  void SetCellHeight(double);
  double GetCellHeight();

  double GetTopPadding() const;
  const double& GetBottomPadding() const;
  void SetTopPadding(double topPadding);
  void SetBottomPadding(double bottomPadding);

  std::shared_ptr<ItemsProvider> GetItemsProvider() const;

  void SetItemsProvider(std::shared_ptr<ItemsProvider> itemsProvider);

  void SetCellCreateCallback(const std::function<void(std::shared_ptr<Element> cellContainer)>& cellCreateCallback);

private:
  class Cell: public Element
  {
  public:
    Cell(Element::Dependencies elementDependencies, int index);

    void PrepareViewModel() override;
    void Arrange() override;

  private:
    std::weak_ptr<Grid> _grid;
    int                 _index;
  };

private:
  int    _columns                         = 3;
  double _cellHeight;
  double _cellWidth;
  double _offsetPercent                   = 0.0;
  int    _baseItemIndex;
  double _rowOffset;
  double _lastHeightUsedForScrollCheck    = 0.0;
  int    _lastItemCountUsedForScrollCheck = 0;
  double _topPadding                      = 0.0;
  double _bottomPadding                   = 0.0;

  std::shared_ptr<ItemsProvider>                _itemsProvider;
  std::function<void()>                         _thumbDataChangeCallback;
  std::function<void(std::shared_ptr<Element>)> _cellCreateCallback;

protected:
  // Cleanup
  void OnElementIsBeingRemoved() override;
};
}

