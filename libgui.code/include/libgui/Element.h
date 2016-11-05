#pragma once 
#include "ViewModelBase.h"
#include "Location.h"
#include "Point.h"
#include "Types.h"
#include "Rect.h"
#include <boost/optional.hpp>
#include <deque>
#include <memory>

namespace libgui
{
// Override conflicting Windows.h definitions
#undef GetFirstChild
#undef GetPrevSibling
#undef GetNextSibling

class ElementManager;
class Element;
class Layer;

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

class Element: public std::enable_shared_from_this<Element>
{
    friend class ElementManager;
public:
    // -----------------------------------------------------------------
    // Constructors
    Element();

    Element(const std::string& typeName);

    // -----------------------------------------------------------------
    // Objects shared among multiple elements

    ElementManager* GetElementManager() const;
    Layer         * GetLayer() const;

    // -----------------------------------------------------------------
    // View Model

    void                           SetViewModel(std::shared_ptr<ViewModelBase>);
    std::shared_ptr<ViewModelBase> GetViewModel();

    // Called during each arrange cycle to set or update the attached view model
    // (unless the PrepareViewModel method is overridden)
    void SetSetViewModelCallback(const std::function<void(std::shared_ptr<Element>)>&);


    // -----------------------------------------------------------------
    // Visual tree

    void RemoveChildren();
    void RemoveChild(std::shared_ptr<Element>);
    void AddChild(std::shared_ptr<Element>);
    void SetSingleChild(std::shared_ptr<Element> child);
    int  GetChildrenCount();

    // -----------------------------------------------------------------
    // Cache Management

    void         ClearCacheAll(int cacheLevel);
    virtual void ClearCacheThis(int cacheLevel);

    // -----------------------------------------------------------------
    // Arrangement

    std::shared_ptr<Element> GetParent();
    std::shared_ptr<Element> GetSingleChild();
    std::shared_ptr<Element> GetFirstChild();
    std::shared_ptr<Element> GetLastChild();
    std::shared_ptr<Element> GetPrevSibling();
    std::shared_ptr<Element> GetNextSibling();

    // -----------------------------------------------------------------
    // Arrange cycle
    // -------------
    // The cycle of arranging and drawing is what powers the display logic of this library.
    // The arrangement determines the location, size and custom logic of elements, while
    // drawing is just drawing.  Individual elements can be updated to reflect new positions,
    // sizes or display data.  If the elements move or change size during an update, then all
    // children are automatically re-arranged.  If the element has arrange dependents then
    // those are also updated.
    // Regarding updating of descendants, the default update logic is that
    // an updated element's descendents will be rearranged only if the
    // element is moved or resized, otherwise its descendants will be redrawn
    // and not rearranged.  This behavior can be changed by setting
    // SetUpdateRearrangesDescendants to true.

    // Specifies the type of update to be performed.  It is important to specify the correct type of
    // update so that the arranging and drawing logic will work correctly and provide the best
    // performance.
    enum class UpdateType
    {
        // Indicates that the element is in the process of being added to the element hierarchy
            Adding,

        // Indicates that the element is in the process of being removed from the element hierarchy
            Removing,

        // Indicates that the position, size or other data contributing to arrangement or drawing
        // has changed
            Modifying,

        // Update the whole element tree at once.  This is reserved for internal use by
        // the ElementManager class.
            Everything
    };

    // Updates this element and all its dependents.
    void Update(UpdateType updateType);

    // Called during each arrange cycle to set or update the position and size of the element
    // (unless the Arrange method is overridden)
    void SetArrangeCallback(const std::function<void(std::shared_ptr<Element>)>&);

    // Set whether calling update on this element will automatically rearrange all of its descendants.
    // If this is false, descendents are only rearranged when the updated element is moved or resized.
    // Descendents of an updated element are always redrawn regardless of this setting.
    void SetUpdateRearrangesDescendants(bool updateRearrangesDescendents);

    // Get whether calling update on this element will automatically rearrange all of its descendants.
    // If this is false, descendents are only rearranged when the updated element is moved or resized.
    // Descendents of an updated element are always redrawn regardless of this setting.
    bool GetUpdateRearrangesDescendants();

    // Add an element which will be updated whenever this element is arranged
    // in such a way that its position or size is modified
    void AddArrangeDependent(std::shared_ptr<Element> dependent);

    // -----------------------------------------------------------------
    // Bounds

    // Set visual bounds, if drawing outside the active area of the element
    void SetVisualBounds(const boost::optional<Rect4>& bounds);

    // Returns the visual bounds outside the active area of the element (if any)
    const boost::optional<Rect4>& GetVisualBounds();

    // Get the total bounds, including any excess visual bounds, of this element
    const Rect4 GetTotalBounds();

    // Gets the bounds of this element used for arrangement and hit testing
    Rect4 GetBounds();

    // -----------------------------------------------------------------
    // Dirty
    void BeginDirtyTrackingIfApplicable(UpdateType updateType);
    const Rect4& EndDirtyTracking(UpdateType updateType, bool& moved);

    // -----------------------------------------------------------------
    // Optional post-construction initialization pass that some elements require

    // Initializes this element and all its descendants
    void InitializeAll();

    // Initializes this element and returns whether this is the first initialization
    virtual bool InitializeThis();

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

    virtual void Draw(const boost::optional<Rect4>& updateArea);

    // Called during each arrange cycle to draw this element (unless the Draw method is overridden)
    void SetDrawCallback(const std::function<void(Element* e, const boost::optional<Rect4>& updateArea)>&);


    // -----------------------------------------------------------------
    // Hit testing

    ElementQueryInfo GetElementAtPoint(const Point& point);

    // -----------------------------------------------------------------
    // Queries

    bool ThisOrAncestors(const std::function<bool(Element*)>& predicate);

    // It is strongly recommended that this method be overridden in each container class
    // in order to increase efficiency of hit testing, assuming that the container class
    // has a more optimized mechanism for locating its children than this
    // default brute force search
    virtual Element* FindLastChild(const Point& point);

    // Returns whether this element intersects with the specified region
    bool Intersects(const Rect4& region);

    // Returns whether the TotalBounds of this element intersects with the specified region
    bool TotalBoundsIntersects(const Rect4& region);

    // Returns whether this element intersects with the specified point
    bool Intersects(const Point& point);

    // -----------------------------------------------------------------
    // Visitors

    // Visit children first to last
    void VisitChildren(const std::function<void(Element*)>& action);

    // Visit ancestors of this element, oldest ancestor first
    void VisitAncestors(const std::function<void(Element*)>& action);

    void VisitArrangeDependents(const std::function<void(Element*)>& action);

    void VisitThisAndDescendents(const std::function<bool(Element*)>& preChildrenAction,
                                 const std::function<void(Element*)>& postChildrenAction);

    void VisitThisAndDescendents(const std::function<void(Element*)>& action);

    void VisitThisAndDescendents(const Rect4& region,
                                 const std::function<bool(Element*)>& preChildrenAction,
                                 const std::function<void(Element*)>& postChildrenAction);

    // It is strongly recommended that this method be overridden in each container class
    // in order to increase efficiency of inter-layer element updates, assuming that the
    // container class has a more optimized mechanism for locating its children than this
    // default brute force search
    virtual void VisitChildren(const Rect4& region, const std::function<void(Element*)>& action);

    // It is strongly recommended that this method be overridden in each container class
    // in order to increase efficiency of hit testing, assuming that the container class
    // has a more optimized mechanism for locating its children than this
    // default brute force search
    virtual void VisitChildrenWithTotalBounds(const Rect4& region, const std::function<void(Element*)>& action);

    // -----------------------------------------------------------------
    // Debugging
    virtual std::string GetTypeName();
    void                SetTypeName(const std::string& name);

    // -----------------------------------------------------------------
    // Destructor
    virtual ~Element();

protected:

    // -----------------------------------------------------------------
    // Arrange cycle

    virtual void ResetArrangement();
    virtual void PrepareViewModel();
    virtual void Arrange();


private:
    // -----------------------------------------------------------------
    // Objects shared among multiple elements

    ElementManager* _elementManager;
    Layer         * _layer;

    // -----------------------------------------------------------------
    // View model

    std::shared_ptr<ViewModelBase> _viewModel;

    // -----------------------------------------------------------------
    // Arrange cycle

    std::deque<std::weak_ptr<Element>> _arrangeDependents;

    bool _updateRearrangesDescendents  = false;

    // -----------------------------------------------------------------
    // Visual tree

    std::shared_ptr<Element> _parent;
    std::shared_ptr<Element> _firstChild;
    std::shared_ptr<Element> _lastChild;
    std::shared_ptr<Element> _prevsibling;
    std::shared_ptr<Element> _nextsibling;
    int                      _childrenCount = 0;

    // -----------------------------------------------------------------
    // Arrangement

    std::function<void(std::shared_ptr<Element>)>
                             _setViewModelCallback;
    std::function<void(std::shared_ptr<Element>)>
                             _arrangeCallback;

    // -----------------------------------------------------------------
    // Bounds

    boost::optional<Rect4> _visualBounds;

    // -----------------------------------------------------------------
    // Dirty

    Rect4 _dirtyBounds      = Rect4(0, 0, 0, 0);
    Rect4 _dirtyTotalBounds = Rect4(0, 0, 0, 0);

    // -----------------------------------------------------------------
    // State tracking

    bool _initialized   = false;
    bool _clipToBounds  = false;
    bool _isVisible     = true;
    bool _isEnabled     = true;
    bool _consumesInput = true;
    bool _isDetached    = false;

    // -----------------------------------------------------------------
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

    // -----------------------------------------------------------------
    // Debugging
    std::string _typeName;

    // -----------------------------------------------------------------
    // Drawing

    std::function<void(Element*, const boost::optional<Rect4>&)>
         _drawCallback;

    // -----------------------------------------------------------------
    // Hit Testing

    ElementQueryInfo GetElementAtPointHelper(const Point& point, bool hasDisabledAncestor);

    // -----------------------------------------------------------------
    // Helper methods

    bool CoveredByLayerAbove(const Rect4& region);
    void RedrawThisAndDescendents(const boost::optional<Rect4>& redrawRegion);

    void VisitAncestorsHelper(const std::function<void(Element*)>& action, bool isCallee);

    void DoArrangeTasks();

    // Returns whether the element is visible
    bool DoDrawTasksIfVisible(const boost::optional<Rect4>& updateArea);

    // Cleans up whatever state was modified by DoDrawTasksIfVisible.
    void DoDrawTasksCleanup();

    bool ClipToBoundsIfNeeded();

    void UpdateHelper(UpdateType updateType);
    void ArrangeAndDrawHelper();
};
}
