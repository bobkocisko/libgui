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

    const std::shared_ptr<ItemsProvider>& GetItemsProvider() const;

    void SetItemsProvider(const std::shared_ptr<ItemsProvider>& itemsProvider);

    void SetCellCreateCallback(const std::function<void(std::shared_ptr<Element> cellContainer)>& cellCreateCallback);

    virtual std::string GetTypeName() override;
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

    std::shared_ptr<ItemsProvider>                _itemsProvider;
    std::function<void(std::shared_ptr<Element>)> _cellCreateCallback;

    class Cell: public Element
    {
    public:
        Cell(const std::shared_ptr<Grid>& parent, int index);

        void PrepareViewModel() override;
        void Arrange() override;

        virtual std::string GetTypeName() override;
    private:
        std::shared_ptr<Grid> _grid;
        int                   _index;
    };
};
}

