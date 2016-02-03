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

    // The following are to be used only internally
    bool IsAtopBusy();
    void SetIsActive(bool isActive);
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

    void* _stateMachine;
    ElementQueryInfo _atopElementInfo;
    Control* _activeControl;

    bool      _isDown;
    bool      _isActiveControlEngaged;
    bool      _shouldUpdateScreen;
    Point     _point;
    InputType _inputType;
    bool      _isDebugLoggingEnabled;
    bool      _isActive;

    std::list<LogEntry> _debugLogEntries;

    template<class Event>
    void ProcessEvent(Event const& evt);
    Control* GetAtopEnabledControl();
};

}