#pragma once
#include "Element.h"
#include "Scrollbar.h"
#include "ItemsProvider.h"

namespace libgui
{
class Grid: public Element, public ScrollDelegate
{

public:
    Grid();

    void Arrange() override;

    double GetCurrentOffsetPercent() override;
    double GetThumbSizePercent() override;
    void   MoveToOffsetPercent(double offsetPercent) override;

    bool CanScroll();

    const int& GetColumns() const;
    void SetColumns(int columns);

    void   SetCellHeight(double);
    double GetCellHeight();

    const double& GetTopPadding() const;
    const double& GetBottomPadding() const;
    void SetTopPadding(double topPadding);
    void SetBottomPadding(double bottomPadding);

    const shared_ptr<ItemsProvider>& GetItemsProvider() const;

    void SetItemsProvider(const shared_ptr<ItemsProvider>& itemsProvider);

    void SetCellCreateCallback(const function<void(shared_ptr<Element> cellContainer)>& cellCreateCallback);


private:
    int    _columns                      = 3;
    double _cellHeight;
    double _cellWidth;
    double _offsetPercent                = 0.0;
    int    _baseItemIndex;
    double _rowOffset;
    double _lastHeightUsedForScrollCheck = 0.0;
    double _topPadding                   = 0.0;
    double _bottomPadding                = 0.0;

    shared_ptr<ItemsProvider>           _itemsProvider;
    function<void(shared_ptr<Element>)> _cellCreateCallback;

    class Cell: public Element
    {
    public:
        Cell(const shared_ptr<Grid>& parent, int index);

        void PrepareViewModel() override;
        void Arrange() override;
    private:
        shared_ptr<Grid> _grid;
        int              _index;
    };
};
}

