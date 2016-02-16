//
// Created by sjm-kociskobd on 2/16/16.
//

#include "libgui/Layer.h"

namespace libgui
{

const boost::optional<Rect4>& Layer::GetOpaqueArea()
{
    return _opaqueArea;
}

Layer* Layer::GetLayerAbove()
{
    return _layerAbove;
}

Layer* Layer::GetLayerBelow()
{
    return _layerBelow;
}

bool Layer::OpaqueContainsRegion(const Rect4& region)
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