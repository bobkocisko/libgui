#pragma once 
#include "ViewModelBase.h"
#include "Location.h"
#include "Point.h"

using namespace std;

namespace libgui
{
// Override conflicting Windows.h definitions
#undef GetFirstChild
#undef GetPrevSibling
#undef GetNextSibling

struct inches
{
    explicit inches(double value);

    operator double();
    inches operator+(inches& other);
    inches operator-(inches& other);
    inches operator*(inches& other);
    inches operator/(inches& other);

private:
    double value;
};

class ElementManager;

class Element: public enable_shared_from_this<Element>
{
public:
    // Element Manager
    void SetElementManager(ElementManager* elementManager);
    ElementManager* GetElementManager() const;

    // View Model
    void                      SetViewModel(shared_ptr<ViewModelBase>);
    shared_ptr<ViewModelBase> GetViewModel();

    // Visual tree
    void RemoveChildren();
    void AddChild(shared_ptr<Element>);
    void SetSingleChild(shared_ptr<Element> child);
    int  GetChildrenCount();

    // Cache Management
    virtual void ClearCache(int cacheLevel);
    virtual void ClearElementCache(int cacheLevel);

    // Arrangement
    shared_ptr<Element> GetParent();
    shared_ptr<Element> GetSingleChild();
    shared_ptr<Element> GetFirstChild();
    shared_ptr<Element> GetLastChild();
    shared_ptr<Element> GetPrevSibling();
    shared_ptr<Element> GetNextSibling();
    virtual void        PrepareViewModel();
    virtual void        Arrange();
    virtual void        ResetArrangement();
    virtual void        ArrangeAndDraw(bool draw);

    void SetSetViewModelCallback(function<void(shared_ptr<Element>)>);
    void SetArrangeCallback(function<void(shared_ptr<Element>)>);

    void SetIsVisible(bool isVisible);
    bool GetIsVisible();
    void SetClipToBounds(bool clipToBounds);
    bool GetClipToBounds();

    void SetLeft(double left);
    void SetTop(double top);
    void SetRight(double right);
    void SetBottom(double bottom);
    void SetCenterX(double centerX);
    void SetCenterY(double centerY);
    void SetWidth(double width);
    void SetHeight(double height);

    void SetLeft(inches left);
    void SetTop(inches top);
    void SetRight(inches right);
    void SetBottom(inches bottom);
    void SetCenterX(inches centerX);
    void SetCenterY(inches centerY);
    void SetWidth(inches width);
    void SetHeight(inches height);

    double GetLeft();
    double GetTop();
    double GetRight();
    double GetBottom();
    double GetCenterX();
    double GetCenterY();
    double GetWidth();
    double GetHeight();

    inches GetLeftInches();
    inches GetTopInches();
    inches GetRightInches();
    inches GetBottomInches();
    inches GetCenterXInches();
    inches GetCenterYInches();
    inches GetWidthInches();
    inches GetHeightInches();

    // Drawing
    virtual void Draw();
    void         SetDrawCallback(function<void(shared_ptr<Element>)>);

    // Hit testing
    Element* GetElementAtPoint(Point);

    virtual ~Element();

private:
    // Element manager
    ElementManager* _elementManager;

    shared_ptr<ViewModelBase> _viewModel;

    // Visual tree
    shared_ptr<Element> _parent;
    shared_ptr<Element> _firstChild;
    shared_ptr<Element> _lastChild;
    shared_ptr<Element> _prevsibling;
    shared_ptr<Element> _nextsibling;
    int                 _childrenCount = 0;

    // Arrangement
    function<void(shared_ptr<Element>)>
                        _setViewModelCallback;
    function<void(shared_ptr<Element>)>
                        _arrangeCallback;

    bool _clipToBounds = false;
    bool _isVisible    = true;

    double _left    = 0;
    double _top     = 0;
    double _right   = 0;
    double _bottom  = 0;
    double _centerX = 0;
    double _centerY = 0;
    double _width   = 0;
    double _height  = 0;

    bool _isLeftSet    = false;
    bool _isTopSet     = false;
    bool _isRightSet   = false;
    bool _isBottomSet  = false;
    bool _isCenterXSet = false;
    bool _isCenterYSet = false;
    bool _isWidthSet   = false;
    bool _isHeightSet  = false;

    // Drawing
    function<void(shared_ptr<Element>)>
         _drawCallback;
};
}
