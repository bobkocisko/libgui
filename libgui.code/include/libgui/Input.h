//
// Created by Bob Kocisko on 12/18/15.
//

#pragma once


#include "Point.h"
#include "Control.h"
#include "Element.h"
#include "InputIdentifier.h"
#include <list>
#include <boost/any.hpp>

namespace libgui
{

class Input
{
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

    // Internal use only
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
    bool IsAtopControl();
    bool TargetIsEnabled();
    bool TargetIsBusy();
    bool IsAtopTarget();
    bool IsPointer();
    bool IsTouch();
    boost::any& GetActiveEvent();

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
    boost::any       _activeEvent;

    std::list<LogEntry> _debugLogEntries;
    template<class Event>
    void                ProcessEvent(Event const& evt);

    void CheckTargetEnabledStatus();
    bool CheckTargetEnabledStatusHelper() const;
};

}