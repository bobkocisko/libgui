//
// Created by sjm-kociskobd on 12/18/15.
//

#include "libgui/Input.h"

namespace libgui
{

Input::Input(const InputId& inputId)
    : _inputId(inputId)
{
    _isDown = false;

    if (_inputId.IsPointer())
    {
        _inputType = InputType::Pointer;
    }
    else
    {
        _inputType = InputType::Touch;
    }
}

/*
 *
 * Core logic
 *
 *
 */

bool Input::NotifyMove(Point point, Element* overElement)
{
    bool shouldUpdateScreen = false;

    if (_activeControl)
    {
        if (overElement == _activeControl)
        {
            if (ActiveControlMove(point))
            {
                shouldUpdateScreen = true;
            }
        }
        else
        {
            if (LeaveActiveControl(point))
            {
                shouldUpdateScreen = true;
            }
        }
    }
    else
    {
        if (overElement)
        {
            Control* overControl = dynamic_cast<Control*>(overElement);
            if (overControl)
            {
                if (EnterControl(point, overControl))
                {
                    shouldUpdateScreen = true;
                };
            }
        }
    }

    _point = point;

    return shouldUpdateScreen;
}

bool Input::NotifyDown()
{
    bool shouldUpdateScreen = false;
    if (_activeControl)
    {
        shouldUpdateScreen = ActiveControlDown(_point);

        // Consider the input captured when it goes down on a control
        _isCapturedByActiveControl = true;
    }

    return shouldUpdateScreen;
}

bool Input::NotifyUp()
{
    bool shouldUpdateScreen = false;
    if (_activeControl)
    {
        shouldUpdateScreen = ActiveControlUp(_point);

        // Consider the input capture released whenever the input goes up
        if (_isCapturedByActiveControl)
        {
            _activeControl = nullptr;
            _isCapturedByActiveControl = false;
        }
    }

    return shouldUpdateScreen;
}



/*
 * Helper functions
 */

bool Input::LeaveActiveControl(Point point)
{
    bool shouldUpdateScreen;

    _activeControl->NotifyInput(InputAction::Leave, _inputType, point, shouldUpdateScreen);

    if (!_isCapturedByActiveControl)
    {
        _activeControl = nullptr;
    }

    return shouldUpdateScreen;
}

bool Input::ActiveControlMove(Point point)
{
    bool shouldUpdateScreen;

    _activeControl->NotifyInput(InputAction::Move, _inputType, point, shouldUpdateScreen);

    return shouldUpdateScreen;
}

bool Input::EnterControl(Point point, Control* control)
{
    bool shouldUpdateScreen;

    InputAction action;
    if (_isDown)
    {
        action = InputAction::EnterPushed;
    }
    else
    {
        action = InputAction::EnterReleased;
    }

    _activeControl = control;
    _activeControl->NotifyInput(action, _inputType, point, shouldUpdateScreen);

    return shouldUpdateScreen;
}

bool Input::ActiveControlDown(Point point)
{
    bool shouldUpdateScreen;

    _activeControl->NotifyInput(InputAction::Push, _inputType, point, shouldUpdateScreen);

    return shouldUpdateScreen;
}

bool Input::ActiveControlUp(Point point)
{
    bool shouldUpdateScreen;

    _activeControl->NotifyInput(InputAction::Release, _inputType, point, shouldUpdateScreen);

    return shouldUpdateScreen;
}
}