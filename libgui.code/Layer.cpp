//
// Created by Bob Kocisko on 2/16/16.
//

#include "libgui/Layer.h"

namespace libgui
{

Layer::Layer()
{

}

void Layer::SetOpaqueArea(const boost::optional<Rect4>& opaqueArea)
{
    _opaqueArea = opaqueArea;
}

const boost::optional<Rect4>& Layer::GetOpaqueArea()
{
    return _opaqueArea;
}

void Layer::VisitLowerLayersIf(const std::function<bool(Layer* currentLayer)>& continueDownPredicate,
                               const std::function<void(Layer* lowerLayer)>& action)
{
    VisitLowerLayersIfHelper(continueDownPredicate, action, true);
}

void Layer::VisitLowerLayersIfHelper(const std::function<bool(Layer* currentLayer)>& continueDownPredicate,
                                     const std::function<void(Layer* lowerLayer)>& action, bool isFirst)
{
    if (continueDownPredicate(this))
    {
        auto nextLayerBelow = GetLayerBelow();
        if (nextLayerBelow)
        {
            nextLayerBelow->VisitLowerLayersIfHelper(continueDownPredicate, action, false);
        }
    }

    // Only perform this on the lower layers, not on the layer that launched this operation
    if (!isFirst)
    {
        action(this);
    }
}

void Layer::VisitHigherLayers(const std::function<void(Layer*)>& action)
{
    VisitHigherLayersHelper(action, true);
}

void Layer::VisitHigherLayersHelper(const std::function<void(Layer*)>& action, bool isFirst)
{
    // Only perform this on the higher layers, not on the layer that launched this operation
    if (!isFirst)
    {
        action(this);
    }

    auto nextLayerAbove = GetLayerAbove();
    if (nextLayerAbove)
    {
        nextLayerAbove->VisitHigherLayersHelper(action, false);
    }
}

Layer* Layer::GetLayerAbove()
{
    return _layerAbove;
}

bool Layer::AnyLayersAbove()
{
    return bool(_layerAbove);
}

Layer* Layer::GetLayerBelow()
{
    return _layerBelow;
}

bool Layer::AnyLayersBelow()
{
    return bool(_layerBelow);
}

bool Layer::OpaqueAreaContains(const Rect4& region)
{
    auto opaqueArea = GetOpaqueArea();
    if (opaqueArea)
    {
        auto& area = opaqueArea.get();

        // Check if this region is fully contained inside
        // the opaque section of the layer
        if (region.left >= area.left &&
            region.top >= area.top &&
            region.right <= area.right &&
            region.bottom <= area.bottom)
        {
            return true;
        }
    }

    return false;
}
}