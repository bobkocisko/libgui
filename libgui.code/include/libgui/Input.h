//
// Created by sjm-kociskobd on 12/18/15.
//

#pragma once


#include "Point.h"
#include "Control.h"
#include "Element.h"
#include "InputId.h"

namespace libgui
{
class Input
{
public:
    Input(const InputId& inputId);

//    enum State
//    {
//        Exists,
//        OverPickable,
//        Captured
//    };
//    State GetState();

    bool NotifyMove(Point point, Element* overElement);
    bool NotifyDown();
    bool NotifyUp();

private:
    InputId _inputId;
//    State _state;

    Control* _activeControl;
    bool      _isCapturedByActiveControl;
    Point     _point;
    InputType _inputType;
    bool      _isDown;

    bool LeaveActiveControl(Point point);
    bool ActiveControlMove(Point point);
    bool EnterControl(Point point, Control* control);
    bool ActiveControlDown(Point point);
    bool ActiveControlUp(Point point);
};

}