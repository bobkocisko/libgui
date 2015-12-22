//
// Created by sjm-kociskobd on 12/18/15.
//

#include "libgui/Input.h"

namespace libgui
{

Input::Input(const InputId& inputId)
    : _inputId(inputId)
{
    _isOverActiveControl = false;
    _isDown              = false;
    _activeControl       = nullptr;
    _ignoredByControl    = nullptr;

    // By default there is no simulation
    _simulationOffset = Point{0, 0};

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

bool Input::NotifyNewPoint(Point point, Element* overElement)
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
    else if (_ignoredByControl)
    {
        if (overElement != _ignoredByControl)
        {
            LeaveIgnoredControl();
        }
    }
    else
    {
        if (overElement)
        {
            Control* overControl = dynamic_cast<Control*>(overElement);
            if (overControl && overControl->GetIsEnabled())
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

    _isDown = true;

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

    _isDown = false;

    if (_activeControl)
    {
        if (ActiveControlUp(_point))
        {
            shouldUpdateScreen = true;
        }

        if (_isCapturedByActiveControl)
        {
            _isCapturedByActiveControl = false;

            if (_isOverActiveControl)
            {
                if (InputType::Touch == _inputType)
                {
                    // The finger has released so we need to indicate that the control has been left
                    if (LeaveActiveControl(_point))
                    {
                        shouldUpdateScreen = true;
                    }
                }
                else
                {
                    // The Pointer hasn't left the control so we don't do anything here.
                }
            }
            else
            {
                // We're releasing outside the control.  Regardless of touch or pointer type,
                // in this case the control has already been notified of having left so we just need to clean up now.
                _activeControl->SetHasActiveInput(false);
                _activeControl = nullptr;
            }
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

    _activeControl->NotifyInput(_inputType, InputAction::Leave, point, shouldUpdateScreen);

    if (!_isCapturedByActiveControl)
    {
        _activeControl->SetHasActiveInput(false);
        _activeControl = nullptr;
    }

    _isOverActiveControl = false;

    return shouldUpdateScreen;
}

bool Input::ActiveControlMove(Point point)
{
    bool shouldUpdateScreen;

    _activeControl->NotifyInput(_inputType, InputAction::Move, point, shouldUpdateScreen);

    return shouldUpdateScreen;
}

bool Input::EnterControl(Point point, Control* control)
{
    bool shouldUpdateScreen;

    if (control->HasActiveInput())
    {
        // There is already another input for this control so we'll be ignored
        _ignoredByControl = control;
        return false;
    }

    InputAction action;
    if (_isDown)
    {
        action = InputAction::EnterPushed;
    }
    else
    {
        action = InputAction::EnterReleased;
    }

    _activeControl       = control;
    _isOverActiveControl = true;
    control->SetHasActiveInput(true);
    _activeControl->NotifyInput(_inputType, action, point, shouldUpdateScreen);

    return shouldUpdateScreen;
}

bool Input::ActiveControlDown(Point point)
{
    bool shouldUpdateScreen;

    _activeControl->NotifyInput(_inputType, InputAction::Push, point, shouldUpdateScreen);

    return shouldUpdateScreen;
}

bool Input::ActiveControlUp(Point point)
{
    bool shouldUpdateScreen;

    _activeControl->NotifyInput(_inputType, InputAction::Release, point, shouldUpdateScreen);

    return shouldUpdateScreen;
}

void Input::LeaveIgnoredControl()
{
    _ignoredByControl = nullptr;
}

}