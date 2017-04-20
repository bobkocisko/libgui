//
// Created by Bob Kocisko on 2/16/16.
//

#pragma once

#include "Element.h"
#include "Rect.h"

#include <boost/optional.hpp>

namespace libgui
{

class Layer: public Element
{
    friend class ElementManager;

protected:
    // Layer can only be created by ElementManager
    Layer();
public:

    // Set the area of the layer that is fully opaque.  This is an optional
    // and recommended optimization so that layers under this opaque area
    // do not need to redraw themselves.
    void SetOpaqueArea(const boost::optional<Rect4>& opaqueArea);

    // The area of the layer that is fully opaque.  This is an optional
    // and recommended optimization so that layers under this opaque area
    // do not need to redraw themselves.
    const boost::optional<Rect4>& GetOpaqueArea();

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

    // This must be called after all the elements of the layer are created and initialized
    void UpdateEverything();

    // Returns whether UpdateEverything was called for this layer
    bool UpdateEverythingCalled() const;

private:
    bool                   _updatedEverything;
    boost::optional<Rect4> _opaqueArea;

    std::weak_ptr<Layer> _layerAbove;
    std::weak_ptr<Layer> _layerBelow;

    void VisitLowerLayersIfHelper(const std::function<bool(Layer*)>& continueDownPredicate,
                                  const std::function<void(Layer*)>& action, bool isFirst);
    void VisitHigherLayersHelper(const std::function<void(Layer*)>& action, bool isFirst);

};

}

