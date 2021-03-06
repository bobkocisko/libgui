//
// Created by Bob Kocisko on 2/16/16.
//

#pragma once

#include "Element.h"
#include "Rect.h"

#include <boost/optional.hpp>

namespace libgui
{

// Can only be constructed by ElementManager
class LayerDependencies
{
  friend class ElementManager;
  explicit LayerDependencies(ElementManager* elementManager);
public:
  ElementManager* elementManager;
};

class Layer: public Element
{
  friend class ElementManager;

public:
  // Can only be constructed by a class with the ability to create the Dependencies class
  Layer(LayerDependencies dependencies);
  Layer(LayerDependencies dependencies, std::string_view typeName);

private:
  // Can only be called by ElementManager
  void PostConstructInternal();

public:
  // Set the area of the layer that is fully opaque.  This is an optional
  // and recommended optimization so that layers under this opaque area
  // do not need to redraw themselves.
  void SetOpaqueArea(const boost::optional<Rect4>& opaqueArea);

  // The area of the layer that is fully opaque.  This is an optional
  // and recommended optimization so that layers under this opaque area
  // do not need to redraw themselves.
  const boost::optional<Rect4>& GetOpaqueArea();

  // Depending on the type of layer, it is often preferable to disallow
  // fuzzy touch input hit testing from descending past the layer to others
  // if any part of the fuzzy region intersects with this layer.
  void SetCapturesAllIntersectingTouchInput(bool capture);

  // Depending on the type of layer, it is often preferable to disallow
  // fuzzy touch input hit testing from descending past the layer to others
  // if any part of the fuzzy region intersects with this layer.
  bool GetCapturesAllIntersectingTouchInput() const;

  // Visit layers below the current one from bottom to top and perform the
  // specified action on each layer
  void VisitLowerLayersIf(const std::function<bool(Layer* currentLayer)>& continueDownPredicate,
                          const std::function<void(Layer* lowerLayer)>& action);

  // Visit layers above the current one from bottom to top and perform the
  // specified action on each layer
  void VisitHigherLayers(const std::function<void(Layer*)>& action);

  // The layer above this one, if any
  std::shared_ptr<Layer> GetLayerAbove();
  bool AnyLayersAbove();

  // The layer below this one, if any
  std::shared_ptr<Layer> GetLayerBelow();
  bool AnyLayersBelow();

  // Returns whether the opaque area of this layer (if any) contains the specified region
  bool OpaqueAreaContains(const Rect4& region);

  void UpdateAfterAdd() override;

private:
  boost::optional<Rect4> _opaqueArea;
  bool                   _capturesAllIntersectingTouchInput;

  std::weak_ptr<Layer> _layerAbove;
  std::weak_ptr<Layer> _layerBelow;

  void VisitLowerLayersIfHelper(const std::function<bool(Layer*)>& continueDownPredicate,
                                const std::function<void(Layer*)>& action, bool isFirst);
  void VisitHigherLayersHelper(const std::function<void(Layer*)>& action, bool isFirst);

};

}

