//
// Created by sjm-kociskobd on 12/18/15.
//

#pragma once


#include "Point.h"
#include "Control.h"
#include "Element.h"
#include "InputIdentifier.h"

namespace libgui
{
class Input
{
public:
    Input(const InputId& inputId);

    bool NotifyNewPoint(Point point, Element* overElement);
    bool NotifyDown();
    bool NotifyUp();

private:
    InputId _inputId;

    Control* _activeControl;
    Control* _ignoredByControl;
    bool      _isDown;
    bool      _isOverActiveControl;
    bool      _isCapturedByActiveControl;
    Point     _point;
    InputType _inputType;
    Point     _simulationOffset;

    bool LeaveActiveControl(Point point);
    bool ActiveControlMove(Point point);
    bool EnterControl(Point point, Control* control);
    bool ActiveControlDown(Point point);
    bool ActiveControlUp(Point point);
    void LeaveIgnoredControl();
};

}