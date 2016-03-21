//
// Created by sjm-kociskobd on 2/20/16.
//

#pragma once

#include "Rect.h"
#include <boost/optional.hpp>
#include <stack>
#include <functional>

namespace libgui
{

class IntersectionStack
{

public:
    typedef boost::optional<Rect4> OptRegion;

    static const Rect4 EmptyRegion;

    void PushRegion(const Rect4& region);

    void PopRegion();

    void SetRegionChangedCallback(const std::function<void(const OptRegion&)>& regionChangedCallback);

private:
    std::stack<Rect4> _stack;

    boost::optional<OptRegion>            _lastNotification;
    std::function<void(const OptRegion&)> _regionChangedCallback;

    void OnRegionChanged(const OptRegion& region);

    void SignalChangeIfNeeded(const OptRegion& region);

};

}