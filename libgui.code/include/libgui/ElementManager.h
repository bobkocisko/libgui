#pragma once

#include "CallPostConstructIfPresent.h"
#include "Control.h"
#include "Element.h"
#include "Input.h"
#include "Layer.h"

#include <vector>
#include <list>
#include <boost/optional.hpp>
#include <queue>

namespace libgui
{

class Layer;

class ElementManager: public std::enable_shared_from_this<ElementManager>
{
public:
  ElementManager();

  // -------------------------------------------------------------------------------------
  // Size
  // -----
  // The ElementManager has an overall size which typically matches the window size
  // of the window that it manages.  There should be a one-to-one relationship between
  // the ElementManager and a window.  Whenever the window is resized, the ElementManager's
  // size should be updated to reflect the new size.

  // Sets the overall size of the ElementManager.  Typically this is the window size.
  void SetSize(const Size& size);

  // Returns the overall size of the ElementManager.  Typically this is the window size.
  const Size& GetSize() const;

  // Returns the overall width of the ElementManager.  Typically this is the window width.
  double GetWidth() const;

  // Returns the overall height of the ElementManager.  Typically this is the window height.
  double GetHeight() const;

  // -------------------------------------------------------------------------------------
  // Layers
  // ------------------
  // Layers are the starting point where elements can be added to an ElementManager
  // instance.  Each layer represents a set of elements that are non-overlapping,
  // whereas layers themselves are meant to overlap other layers in a performant way.

  /***
   * CreateLayerAbove
   *
   * Create and add a new layer above the specified one.
   *
   * @tparam LayerType
   * The type of the layer that will be created.  By default the root class 'Layer'
   * is used.
   * @param existing
   * An optional existing layer above which to add the new layer.
   * If this is nullptr, the layer will be added on top of all other layers.
   * @return
   * Returns the new layer which has been created by this function.
   */
  template<class LayerType=Layer, class... LayerArgs>
  std::shared_ptr<LayerType> CreateLayerAbove(std::shared_ptr<Layer> existing, LayerArgs&& ... args)
  {
    auto layer = std::make_shared<LayerType>(LayerDependencies{this}, std::forward<LayerArgs>(args)...);
    layer->PostConstructInternal();
    CallPostConstructIfPresent(layer);
    AddLayerAbove(existing, layer);
    return layer;
  }

  /***
   * CreateLayerBelow
   *
   * Create and add a new layer below the specified one.
   *
   * @tparam LayerType
   * The type of the layer that will be created.  By default the root class 'Layer'
   * is used.
   * @param existing
   * An optional existing layer below which to add the new layer.
   * If this is nullptr, the layer will be added on top of all other layers.
   * @return
   * Returns the new layer which has been created by this function.
   */
  template<class LayerType=Layer, class... LayerArgs>
  std::shared_ptr<LayerType> CreateLayerBelow(std::shared_ptr<Layer> existing, LayerArgs&& ... args)
  {
    auto layer = std::make_shared<LayerType>(LayerDependencies{this}, std::forward<LayerArgs>(args)...);
    layer->PostConstructInternal();
    CallPostConstructIfPresent(layer);
    AddLayerBelow(existing, layer);
    return layer;
  }

  // RemoveLayer
  // -----------
  // Removes the specified layer, automatically performing an update during the removal
  void RemoveLayer(std::shared_ptr<Layer> layer);

  // -------------------------------------------------------------------------------------
  // Arranging and drawing
  // ------------------
  // The cycle of arranging and drawing is what powers the display logic of this library.
  // The arrangement determines the location, size and custom logic of elements, while
  // drawing is just drawing.  There are two types of arrangement and drawing supported.
  // The first is a brute force mechanism whereby all the elements in all layers are
  // arranged and drawn.  This should not be necessary except at application startup or
  // when the parent window itself is resized.  The other type is an update.  Individual
  // elements can be updated one at a time to reflect new positions, sizes or display
  // data.  If the elements move or change size during an update, then all children are
  // automatically re-arranged.  If the element has arrange dependents then those are
  // also updated.

  void UpdateEverything();

  // -------------------------------------------------------------------------------------
  // Input notification
  // ------------------
  // This library expects that there will be support for pointer input and possibly
  // also single or multiple touch inputs.  The expectation is that the client of
  // this library will map the actual events for each platform to these three methods,
  // which will trigger most of the actions of the library.

  void NotifyNewPoint(InputId inputId, Point point);
  void NotifyDown(InputId inputId);
  void NotifyUp(InputId inputId);

  // Set the fuzzy touch size (defaults to 30x30 pixels)
  void SetFuzzyTouchSize(const Size& size);

  // Return the last point notification for the specified InputId, or (-1, -1) if
  // no point has been notified for that input yet.
  const Point& GetCurrentPoint(InputId inputId);

  void SetSystemCaptureCallback(const std::function<void(bool)>& systemCaptureCallback);

  // -------------------------------------------------------------------------------------
  // Clipping support
  // ----------------
  // This library expects that there will be support for a clip stack,
  // meaning that multiple clip rectangles can be pushed onto the stack or popped off
  // the stack, with each new clip rectangle decreasing the current clip area.
  // For an example of an OpenGL implementation of such a stack, refer to the examples
  // in this library.

  void SetPushClipCallback(const std::function<void(const Rect4&)>& callback);
  void SetPopClipCallback(const std::function<void()>& callback);

  void PushClip(const Rect4& clip);
  void PopClip();

  // -------------------------------------------------------------------------------------
  // Inches to Pixels conversion
  // ---------------------------

  // DPI: The number of pixels per inch, used to support setting element values in inches
  double GetDpiX() const;
  // DPI: The number of pixels per inch, used to support setting element values in inches.
  // If you need a per-monitor DPI setting you need to create a separate ElementManager for each monitor.
  void SetDpiX(double dpi);
  // DPI: The number of pixels per inch, used to support setting element values in inches
  double GetDpiY() const;
  // DPI: The number of pixels per inch, used to support setting element values in inches.
  // If you need a per-monitor DPI setting you need to create a separate ElementManager for each monitor.
  void SetDpiY(double dpiY);

  // -------------------------------------------------------------------------------------
  // Redrawn region
  // ---------------
  // When using OpenGL as a backend and employing double-buffering, there is a need to
  // copy any changes from one buffer to the other so that the two buffers stay synchronized
  // over time.  The redrawn region tracks all the areas of any element associated with
  // this element manager in any layer and so provides the entire rectangular area that
  // would need to be copied between buffers whenever that is performed.

  // Remove any current redrawn region in order to begin tracking changes
  void ClearRedrawnRegion();

  // Used internally by elements to indicate that a new region has been redrawn
  void AddToRedrawnRegion(const Rect4& region);

  // Returns the total region that has been redrawn since the last call to ClearRedrawnRegion
  const boost::optional<Rect4>& GetRedrawnRegion();

  // -------------------------------------------------------------------------------------
  // Debugging visualization
  // -----------------------
  // It is useful when debugging the logic or some of the touch hardware to be able to see
  // on screen where the inputs are being reported as occurring

  const std::vector<Input*>& GetActiveInputs() const;
  void EnableDebugLogging();
  Input* GetInput(const InputId& inputId);

  // -------------------------------------------------------------------------------------

  // -------------------------------------------------------------------------------------
  // Update tracking
  // ---------------
  // During the update cycle it is possible to have circular references between elements
  // which list each other as arrange dependents.  To avoid a stack overflow exception
  // due to endless circular references, it is necessary to track which elements have
  // been updated so far.

  // Internal use only.  Clears the list of updated elements at the beginning of an update pass
  void ClearUpdateTracking();

  // Internal use only.  Checks and returns whether the specified element has been updated since the
  // last call to ClearUpdateTracking, and if it hasn't, adds the element to the tracking list
  bool TryBeginUpdate(Element* element);


private:
  typedef std::list<std::shared_ptr<Layer>> LayerList;

  std::vector<Input*>               _activeInputs;
  LayerList                         _layers;
  std::function<void(bool)>         _systemCaptureCallback;
  bool                              _isDebugLoggingEnabled;
  double                            _dpiX = 96.0;
  double                            _dpiY = 96.0;
  std::function<void(const Rect4&)> _pushClipCallback;
  std::function<void()>             _popClipCallback;
  boost::optional<Rect4>            _redrawnRegion;
  std::deque<Element*>              _updatedElements;
  Size                              _size;
  Size                              _fuzzyTouchSize;

private:
  void AddLayerAbove(std::shared_ptr<Layer> existing,
                           std::shared_ptr<Layer> layerToAdd);

  void AddLayerBelow(std::shared_ptr<Layer> existing,
                           std::shared_ptr<Layer> layerToAdd);

  friend class Control;
  void NotifyControlIsBeingDestroyed(Control* control);

};
}
