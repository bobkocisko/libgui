#pragma once 
#include "ViewModelBase.h"
#include "Location.h"
#include "Point.h"
#include "Types.h"
#include "Rect.h"
#include <boost/optional.hpp>
#include <queue>

class Layer;

using namespace std;

namespace libgui
{
// Override conflicting Windows.h definitions
#undef GetFirstChild
#undef GetPrevSibling
#undef GetNextSibling

class ElementManager;
class Element;

struct ElementQueryInfo
{
    // This constructor is used when nothing is found
    ElementQueryInfo();

    // This constructor is used when an element is found
    ElementQueryInfo(Element* ElementAtPoint, bool HasDisabledAncestor);

    // The element, if any, under the specified point
    Element* ElementAtPoint;

    // Whether any element was found under the specified point
    bool FoundElement();

    // Whether any ancestor is set as disabled
    bool HasDisabledAncestor;
};

class Element: public enable_shared_from_this<Element>
{
    friend class ElementManager;
public:

    // -----------------------------------------------------------------
    // Objects shared among multiple elements

    ElementManager* GetElementManager() const;
    Layer         * GetLayer() const;

    // -----------------------------------------------------------------
    // View Model

    void                      SetViewModel(shared_ptr<ViewModelBase>);
    shared_ptr<ViewModelBase> GetViewModel();

    // Called during each arrange cycle to set or update the attached view model
    // (unless the PrepareViewModel method is overridden)
    void SetSetViewModelCallback(function<void(shared_ptr<Element>)>);


    // -----------------------------------------------------------------
    // Visual tree

    void RemoveChildren();
    void AddChild(shared_ptr<Element>);
    void SetSingleChild(shared_ptr<Element> child);
    int  GetChildrenCount();

    // -----------------------------------------------------------------
    // Cache Management

    virtual void ClearCache(int cacheLevel);
    virtual void ClearElementCache(int cacheLevel);

    // -----------------------------------------------------------------
    // Arrangement

    shared_ptr<Element> GetParent();
    shared_ptr<Element> GetSingleChild();
    shared_ptr<Element> GetFirstChild();
    shared_ptr<Element> GetLastChild();
    shared_ptr<Element> GetPrevSibling();
    shared_ptr<Element> GetNextSibling();

    // -----------------------------------------------------------------
    // Arrange cycle

    virtual void ArrangeAndDraw();
    virtual void Update();
    virtual void ResetArrangement();
    virtual void PrepareViewModel();
    virtual void Arrange();

    // Called during each arrange cycle to set or update the position and size of the element
    // (unless the Arrange method is overridden)
    void SetArrangeCallback(function<void(shared_ptr<Element>)>);

    // -----------------------------------------------------------------
    // Bounds

    // Set visual bounds, if drawing outside the active area of the element
    // The values are relative to the corresponding edge of the element (ie not absolute)
    void SetVisualBounds(const Rect4& bounds);

    // Returns the visual bounds outside the active area of the element (if any)
    // The values are relative to the corresponding edge of the element (ie not absolute)
    const boost::optional<Rect4>& GetVisualBounds();

    // Get the total bounds, including any excess visual bounds, of this element
    const Rect4 GetTotalBounds();

    // -----------------------------------------------------------------
    // Dirty
    void BeginDirtyTracking();
    const Rect4& EndDirtyTracking();

    // -----------------------------------------------------------------
    // Optional post-construction initialization pass that some elements require

    virtual void InitializeAll();
    virtual void InitializeThis();

    // -----------------------------------------------------------------
    // State tracking

    void SetIsVisible(bool isVisible);
    bool GetIsVisible();
    void SetIsEnabled(bool isEnabled);
    bool GetIsEnabled();
    void SetClipToBounds(bool clipToBounds);
    bool GetClipToBounds();
    void SetConsumesInput(bool consumesInput);
    bool GetConsumesInput();

    // -----------------------------------------------------------------
    // Position and size

    void SetLeft(double left);
    void SetTop(double top);
    void SetRight(double right);
    void SetBottom(double bottom);
    void SetCenterX(double centerX);
    void SetCenterY(double centerY);
    void SetWidth(double width);
    void SetHeight(double height);

    void SetLeft(Inches left);
    void SetTop(Inches top);
    void SetRight(Inches right);
    void SetBottom(Inches bottom);
    void SetCenterX(Inches centerX);
    void SetCenterY(Inches centerY);
    void SetWidth(Inches width);
    void SetHeight(Inches height);

    HPixels GetLeft();
    VPixels GetTop();
    HPixels GetRight();
    VPixels GetBottom();
    HPixels GetCenterX();
    VPixels GetCenterY();
    HPixels GetWidth();
    VPixels GetHeight();

    HPixels GetHPixels(Inches in);
    VPixels GetVPixels(Inches in);

    // -----------------------------------------------------------------
    // Drawing

    virtual void Draw();

    // Called during each arrange cycle to draw this element (unless the Draw method is overridden)
    void SetDrawCallback(function<void(shared_ptr<Element>)>);

    // -----------------------------------------------------------------
    // Hit testing

    // It is strongly recommended that this method be overridden in each container class
    // in order to increase efficiency of hit testing, assuming that the container class
    // has a more optimized mechanism for locating its children than this
    // default brute force search
    ElementQueryInfo GetElementAtPoint(Point);

    // -----------------------------------------------------------------
    // Queries

    bool ThisOrAncestors(const std::function<bool(Element*)>& predicate);

    // It is strongly recommended that this method be overridden in each container class
    // in order to increase efficiency of inter-layer element updates, assuming that the
    // container class has a more optimized mechanism for locating its children than this
    // default brute force search
    // Note that the order in which the elements is added to the results list is important:
    // it should be the same order that the elements would be drawn, back to front
    virtual void FindVisibleElements(const Rect4& region, std::queue<Element*>& results);

    // -----------------------------------------------------------------
    // Destructor

    virtual ~Element();

private:
    // Element manager
    ElementManager* _elementManager;

    // Layer
    Layer* _layer;

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

    // Bounds
    boost::optional<Rect4> _visualBounds;

    // Dirty
    Rect4 _dirtyBounds;

    // State tracking
    bool _clipToBounds  = false;
    bool _isVisible     = true;
    bool _isEnabled     = true;
    bool _consumesInput = true;

    // Position and size
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

    // Hit Testing
    ElementQueryInfo GetElementAtPointHelper(Point point, bool hasDisabledAncestor);

    bool CoveredByLayerAbove(const Rect4& region);
    void RearrangeAndRedrawHelper(const Rect4& redrawRegion);
    void UpdateLayersBelow(Layer* layer, const Rect4& redrawRegion);
    void FindVisibleElementsHelper(const Rect4& region, std::list<Element*>& results);
};
}
