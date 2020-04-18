#pragma once

#include "CallPostConstructIfPresent.h"
#include "Location.h"
#include "Point.h"
#include "Rect.h"
#include "Types.h"
#include "ViewModelBase.h"

#include <boost/optional.hpp>
#include <deque>
#include <memory>
#include <string_view>
#include <type_traits>

namespace libgui
{
// Override conflicting Windows.h definitions
#undef GetFirstChild
#undef GetPrevSibling
#undef GetNextSibling

class ElementManager;
class Element;
class Layer;
class LayerDependencies;

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

struct FuzzyHitQuery
{
  FuzzyHitQuery();

  bool FoundFiftyPercent() const;

  ElementQueryInfo MaxMatchingElement;
  double MaxMatchingPercent;
};

class Element: public std::enable_shared_from_this<Element>
{
  friend class ElementManager;

public:
  class Dependencies
  {
    friend class Element;

    // Can only be created via Element::CreateChild<T>
    explicit Dependencies(std::shared_ptr<Element> parent);

  public:
    std::shared_ptr<Element> parent;
  };

  // -----------------------------------------------------------------
  // Constructors
  //
  Element(Dependencies dependencies);
  Element(Dependencies dependencies, std::string_view typeName);
  Element(const LayerDependencies& layerDependencies, std::string_view typeName);

public:
  // -----------------------------------------------------------------
  // Objects shared among multiple elements

  ElementManager* GetElementManager() const;
  std::shared_ptr<Layer> GetLayer() const;

  // -----------------------------------------------------------------
  // View Model

  void SetViewModel(std::shared_ptr<ViewModelBase>);
  std::shared_ptr<ViewModelBase> GetViewModel();

  // Called during each arrange cycle to set or update the attached view model
  // (unless the PrepareViewModel method is overridden)
  void SetSetViewModelCallback(const std::function<void(std::shared_ptr<Element>)>&);


  // -----------------------------------------------------------------
  // Visual tree

  /**
   * CreateChild
   *
   * Create and add a new child to this element.
   *
   * @tparam ChildType
   * @tparam ChildArgs
   * @param args
   * @return
   */
  template<class ChildType, class... ChildArgs>
  std::shared_ptr<ChildType> CreateChild(ChildArgs&& ... args)
  {
    auto child = std::make_shared<ChildType>(Dependencies{shared_from_this()}, std::forward<ChildArgs>(args)...);
    CallPostConstructIfPresent(child);
    AddChildHelper(child);
    return child;
  }

  enum class UpdateWhenRemoving
  {
    Yes,
    No
  };

  void RemoveChildren(UpdateWhenRemoving update);
  void RemoveChild(std::shared_ptr<Element>);
  int GetChildrenCount();

  // -----------------------------------------------------------------
  // Cache Management

  void ClearCacheAll(int cacheLevel);
  virtual void ClearCacheThis(int cacheLevel);

  // -----------------------------------------------------------------
  // Arrangement

  std::shared_ptr<Element> GetParent() const;
  std::shared_ptr<Element> GetSingleChild() const;
  std::shared_ptr<Element> GetFirstChild() const;
  std::shared_ptr<Element> GetLastChild() const;
  std::shared_ptr<Element> GetPrevSibling() const;
  std::shared_ptr<Element> GetNextSibling() const;

  // -----------------------------------------------------------------
  // Arrange cycle
  // -------------
  // The cycle of arranging and drawing is what powers the display logic of this library.
  // The arrangement determines the location, size and custom logic of elements, while
  // drawing is just drawing.  Individual elements can be updated to reflect new positions,
  // sizes or display data.  If the elements move or change size during an update, then all
  // children are automatically re-arranged.
  // Regarding updating of descendants, the default update logic is that
  // an updated element's descendents will be rearranged only if the
  // element is moved or resized, otherwise its descendants will be redrawn
  // and not rearranged.  This behavior can be changed by setting
  // SetUpdateRearrangesDescendants to true.

  /**
   * UpdateAfterAdd
   *
   * Request that this element and its descendants be updated after they have been
   * added to the element tree.
   *
   * After the initial ElementManager::UpdateEverything call, this method must be
   * called on each element that is added to the tree.  The general flow is:
   * 1 Create the new element or element tree
   * 2 Register callbacks on all elements
   * 3 Call this method on the element or root element you created and it will
   *   update that element and all its descendants in one pass
   */
  void UpdateAfterAdd();

  /**
   * Request that this element and its descendants be updated after the position,
   * size or other data contributing to arrangement and/or drawing has changed.
   */
  void UpdateAfterModify();

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

  // Register a later sibling that overlaps this one. Note that this
  // registration does not change the drawing order.  It simply ensures
  // that when this element is being updated, overlapping elements get
  // re-drawn as well.
  void RegisterOverlappingElement(std::shared_ptr<Element> other);

  void UnregisterOverlappingElement(std::shared_ptr<Element> other);


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

  // Sets or gets the touch margin.  This is an unusual use of Rect4: each of
  // left, top, right and bottom are simply positive values that indicate the
  // margin away from the element bounds where touch is considered effective.
  void SetTouchMargin(const Rect4& margin);
  const Rect4& GetTouchMargin() const;

  // -----------------------------------------------------------------
  // State tracking

  void SetIsVisible(bool isVisible);
  bool GetIsVisible();
  bool GetAreAncestorsVisible();
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
  HPixels GetHPixels(double px);
  VPixels GetVPixels(double px);

  // -----------------------------------------------------------------
  // Drawing

  // Called during each arrange cycle to draw this element (unless the Draw method is overridden)
  void SetDrawCallback(const std::function<void(Element* e, const boost::optional<Rect4>& updateArea)>&);


  // -----------------------------------------------------------------
  // Hit testing

  ElementQueryInfo GetElementAtPoint(const Point& point);

  // Returns whether there is any intersection at all with the current element
  bool GetElementInRect(const Rect4& hitRect, FuzzyHitQuery& hitQuery);

  // -----------------------------------------------------------------
  // Queries

  bool ThisOrAncestors(const std::function<bool(Element*)>& predicate);

  bool ThisIsEarlierSiblingOf(Element* other);

  // It is strongly recommended that this method be overridden in each container class
  // in order to increase efficiency of hit testing, assuming that the container class
  // has a more optimized mechanism for locating its children than this
  // default brute force search
  virtual Element* FindLastChild(const Point& point);

  // Returns whether this element intersects with the specified region
  bool Intersects(const Rect4& region);

  // Returns whether the touch region of this element intersects with the
  // specified region (by including the touch margin)
  bool TouchIntersects(const Rect4& region);

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

  void VisitOverlappingElements(const std::function<void(Element*)>& action);
  void VisitOverlappedElements(const std::function<void(Element*)>& action);

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
  virtual void VisitChildren(const Rect4& region, const std::function<bool(Element*)>& action);

  // It is strongly recommended that this method be overridden in each container class
  // in order to increase efficiency of inter-layer element updates, assuming that the
  // container class has a more optimized mechanism for locating its children than this
  // default brute force search
  virtual void VisitLastChildren(const Rect4& region, const std::function<bool(Element*)>& action);

  // It is strongly recommended that this method be overridden in each container class
  // in order to increase efficiency of hit testing, assuming that the container class
  // has a more optimized mechanism for locating its children than this
  // default brute force search
  virtual void VisitChildrenWithTotalBounds(const Rect4& region, const std::function<bool(Element*)>& action);

  // -----------------------------------------------------------------
  // Debugging
  std::string_view GetTypeName() const;
  void SetTypeName(std::string_view name);

  // -----------------------------------------------------------------
  // Destructor
  virtual ~Element()
  {
  }

protected:

  // -----------------------------------------------------------------
  // Arrange cycle

  virtual void ResetArrangement();
  virtual void PrepareViewModel();
  virtual void Arrange();

  // -----------------------------------------------------------------
  // Draw cycle

  virtual void Draw(const boost::optional<Rect4>& updateArea);

  // -----------------------------------------------------------------
  // Life cycle

  // A child class can override this to know when it is being removed
  // from the element tree and perform any necessary cleanup (such as
  // resetting callbacks).  Note that when this is called the remove
  // cycle has already begun and you cannot rely on information about
  // the parent or siblings because in the case of removing a large
  // branch of the element tree, for efficiency purposes the entire
  // branch is removed at once rather than correctly removing each
  // element one at a time and rewiring its surrounding siblings.
  // In that case this method will be called for each element that is
  // removed even though its siblings and parent information may no
  // longer be valid.
  virtual void OnElementIsBeingRemoved();


private:
  // -----------------------------------------------------------------
  // Objects shared among multiple elements

  ElementManager* _elementManager;
  std::shared_ptr<Layer> _layer;

  // -----------------------------------------------------------------
  // View model

  std::shared_ptr<ViewModelBase> _viewModel;

  // -----------------------------------------------------------------
  // Arrange cycle

  bool _inArrangeMethodNow = false;

  struct ArrangeEffects
  {
    bool WasInvisibleBeforeAndAfter() const;
    bool ElementWasMovedOrResized() const;
    bool ElementBecameVisible() const;
    const Rect4& GetUnionedTotalBounds() const;
    bool ChildrenRequestedArrange() const;

    bool  wasInvisibleBeforeAndAfter;
    bool  elementWasMovedOrResized;
    bool  elementBecameVisible;
    Rect4 unionedTotalBounds;
    bool  childrenRequestedArrange;
  };

  struct MonitorArrangeEffects
  {
    MonitorArrangeEffects(
      bool addingElement,
      bool originallyVisible,
      const Rect4& originalBounds,
      const Rect4& originalTotalBounds);

    void NotifyChildRequestedArrange();

    ArrangeEffects Finish(bool currentlyVisible,
                          const Rect4& currentBounds, const Rect4& currentTotalBounds) const;

  private:
    bool  addingElement;
    bool  originallyVisible;
    Rect4 originalBounds;
    Rect4 originalTotalBounds;
    bool  childrenRequestedArrange;
  };

  boost::optional<MonitorArrangeEffects&> _monitoringArrangeEffects;

  std::deque<std::weak_ptr<Element>> _overlappedBy;
  std::deque<std::weak_ptr<Element>> _overlaps;

  bool _updateRearrangesDescendents       = false;

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
  Rect4 _touchMargin;

  // -----------------------------------------------------------------
  // State tracking

  bool _initialUpdate  = false;
  bool _clipToBounds   = false;
  bool _isVisible      = true;
  bool _isEnabled      = true;
  bool _consumesInput  = false;
  bool _isDetached     = false;

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
  std::string_view _typeName;

  // -----------------------------------------------------------------
  // Drawing

  std::function<void(Element*, const boost::optional<Rect4>&)>
              _drawCallback;

  // -----------------------------------------------------------------
  // Hit Testing

  ElementQueryInfo GetElementAtPointHelper(const Point& point, bool hasDisabledAncestor);
  bool GetElementInRectHelper(const Rect4& hitRect, FuzzyHitQuery& hitQuery,
                              bool hasDisabledAncestor);

  // -----------------------------------------------------------------
  // Helper methods

  void AddChildHelper(std::shared_ptr<Element>);

  bool CoveredByLayerAbove(const Rect4& region);
  void RedrawThisAndDescendents(const boost::optional<Rect4>& redrawRegion);

  void VisitAncestorsHelper(const std::function<void(Element*)>& action, bool isCallee);

  void DoArrangeTasks();

  // Returns whether the element is visible
  bool DoDrawTasksIfVisible(const boost::optional<Rect4>& updateArea);

  // Cleans up whatever state was modified by DoDrawTasksIfVisible.
  void DoDrawTasksCleanup();

  bool ClipToBoundsIfNeeded();

  // Specifies the type of update to be performed.  It is important to specify the correct type of
  // update so that the arranging and drawing logic will work correctly and provide the best
  // performance.
  enum class UpdateType
  {
    // Indicates that the element is in the process of being added to the element hierarchy
      Adding,

    // Indicates that the position, size or other data contributing to arrangement or drawing
    // has changed
      Modifying,

    // Indicates that the element is in the process of being removed from the element hierarchy
      Removing,

    // Update the whole element tree at once.
      Everything
  };

  // Updates this element and all its dependents.
  void Update(UpdateType updateType);

  void UpdateHelper(UpdateType updateType);
  void ArrangeAndDrawHelper();

  void SetIsDetached(bool isDetached);

  void RegisterOverlappedElement(std::shared_ptr<Element> other);

  void UnregisterOverlappedElement(std::shared_ptr<Element> other);

protected:
  // For use by the Layer class only
  void SetLayerFieldToSharedFromThis();
};

}
