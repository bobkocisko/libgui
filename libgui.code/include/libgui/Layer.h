//
// Created by sjm-kociskobd on 2/16/16.
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

public:
    // The area of the layer that is fully opaque.  This is an optional
    // and recommended optimization so that layers under this opaque area
    // do not need to redraw themselves.
    const boost::optional<Rect4>& GetOpaqueArea();

    // The layer above this one, if any
    Layer* GetLayerAbove();

    // The layer below this one, if any
    Layer* GetLayerBelow();

    bool OpaqueContainsRegion(const Rect4& region);

private:
    boost::optional<Rect4> _opaqueArea;

    Layer* _layerAbove = nullptr;
    Layer* _layerBelow = nullptr;

};

}

