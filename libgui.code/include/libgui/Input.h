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
    virtual ~Input();

    bool NotifyNewPoint(Point point, Element* atopElement);
    bool NotifyDown();
    bool NotifyUp();

    // The following are to be used only internally
    bool IsAtopBusy();
    bool IsAtopAvailable();
    bool IsAtopNothing();
    void EnterControl();
    void LeaveControl();
    void SaveEngaged();
    void SendNotifyMove();
    void SendNotifyDown();
    void SendNotifyUp();
    void SendNotifyEngagedEscape();
    void SendNotifyEngagedReturn();

private:
    InputId _inputId;

    void   * _stateMachine;
    Control* _atopControl;
    Control* _activeControl;

    bool      _isDown;
    bool      _isActiveControlEngaged;
    bool      _shouldUpdateScreen;
    Point     _point;
    InputType _inputType;

    template<class Event>
    void ProcessEvent(Event const& evt);
};

}