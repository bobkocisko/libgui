//
// Created by sjm-kociskobd on 12/18/15.
//

#pragma once


#include "Point.h"
#include "Control.h"
#include "Element.h"
#include "InputIdentifier.h"
#include <list>

namespace libgui
{

namespace SmInput
{
class PointerStateMachineFrontEnd;
class TouchStateMachineFrontEnd;
}

class Input
{
    friend class SmInput::PointerStateMachineFrontEnd;
    friend class SmInput::TouchStateMachineFrontEnd;

public:
    Input(const InputId& inputId);
    virtual ~Input();

    bool NotifyNewPoint(Point point, ElementQueryInfo elementQueryInfo);
    bool NotifyDown();
    bool NotifyUp();

    // Debugging
    const Point    & GetPoint() const;
    const InputType& GetInputType() const;
    bool                GetIsActive() const;
    struct LogEntry
    {
        Point point;
        bool  IsActive;
    };
    void                EnableDebugLogging();
    std::list<LogEntry> GetRecentDebugLogEntries();

private:
    InputId _inputId;

    void   * _stateMachine;
    Control* _atopControl;
    Control* _target;

    ElementQueryInfo _atopElementInfo;
    bool             _targetEnabledState;
    bool             _isDown;
    bool             _shouldUpdateScreen;
    Point            _point;
    InputType        _inputType;
    bool             _isDebugLoggingEnabled;
    bool             _isActive;

    std::list<LogEntry> _debugLogEntries;

    template<class Event>
    void ProcessEvent(Event const& evt);
    void CheckTargetEnabledStatus();
    void SetIsActive(bool isActive);
    void SendNotifyMove();
    void SendNotifyDown();
    void SendNotifyUp();
    void SendNotifyEngagedEscape();
    void SendNotifyEngagedReturn();
    void SendNotifyEnter();
    void SendNotifyLeave();
    void SendNotifyBusy();
    void SendNotifyAvailable();

    void SetTargetToAtopControl();
    void SetTargetToNothing();
    bool CheckTargetEnabledStatusHelper() const;
    bool IsAtopControl();
    bool TargetIsEnabled();
    bool TargetIsBusy();
    bool IsAtopTarget();
};

}