//
// Created by sjm-kociskobd on 12/18/15.
//

#include "libgui/Input.h"
#include "libgui/Element.h"

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 30
#define BOOST_MPL_LIMIT_MAP_SIZE 30

#include <boost/msm/front/states.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/euml/euml.hpp>

using namespace boost::msm::front;
using namespace boost::msm::back;
using boost::msm::front::euml::func_state;
using emptyvector = boost::fusion::vector<>;
using boost::mpl::vector1;
using boost::mpl::vector2;
using boost::mpl::vector3;
using boost::msm::TerminateFlag;
using boost::msm::front::euml::Or_;

template<class A1, class A2>
using AS2 = ActionSequence_<vector2<A1, A2>>;

template<class A1, class A2, class A3>
using AS3 = ActionSequence_<vector3<A1, A2, A3>>;

namespace libgui
{

namespace SmInput
{
// events
struct Move
{
};
struct Up
{
};
struct Down
{
};
struct TargetBecameDisabled
{
};
struct TargetBecameEnabled
{
};

class PointerStateMachineFrontEnd: public state_machine_def<PointerStateMachineFrontEnd>
{

public:
    PointerStateMachineFrontEnd(Input* parent)
        : _parent(parent)
    {
    }

    // states
    struct Idle: public state<>
    {
        template<class Event, class Fsm>
        void on_entry(Event const& evt, Fsm& fsm)
        {
            fsm._parent->SetIsActive(false);
        }

        template<class Event, class Fsm>
        void on_exit(Event const& evt, Fsm& fsm)
        {
            fsm._parent->SetIsActive(true);
        }
    };
    struct Pending: public state<>
    {
    };
    struct Engaged: public state<>
    {
    };
    struct EngagedRemotely: public state<>
    {
    };
    struct HasBusy: public state<>
    {
    };

    // guards
    struct IsAtopControl
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            return fsm._parent->IsAtopControl();
        }
    };
    struct IsAtopTarget
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            return fsm._parent->IsAtopTarget();
        }
    };
    struct TargetIsBusy
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            return fsm._parent->TargetIsBusy();
        }
    };
    struct TargetIsEnabled
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            return fsm._parent->TargetIsEnabled();
        }
    };

    // actions
    struct NotifyEnter
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->SendNotifyEnter();
        }
    };
    struct NotifyLeave
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->SendNotifyLeave();
        }
    };
    struct NotifyMove
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->SendNotifyMove();
        }
    };
    struct NotifyDown
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->SendNotifyDown();
        }
    };
    struct NotifyUp
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->SendNotifyUp();
        }
    };
    struct NotifyEngagedEscape
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->SendNotifyEngagedEscape();
        }
    };
    struct NotifyEngagedReturn
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->SendNotifyEngagedReturn();
        }
    };

    // Replaces the default no-transition response.
    template<class FSM, class Event>
    void no_transition(Event const& e, FSM&, int state)
    {
        // Simply ignore any event that doesn't generate a transition
    }

    // Set up the starting state of the state machine
    typedef Idle initial_state;


    // Transition table for state machine
    // @formatter:off


    // NOTE: Idle must be the first state listed in this table
    struct transition_table : boost::mpl::vector<
    //    Start                Event                       Next State           Action                         Guard
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < Idle               , MoveAtopAvailableControl  , Pending            , EnterControl                 , none            >,
    Row < Idle               , MoveAtopBusyControl       , HasBusy            , none                         , none            >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < HasBusy            , MoveAtopNothing           , Idle               , none                         , none            >,
    Row < HasBusy            , ControlBecameDisabled     , Idle               , none                         , none            >,
    Row < HasBusy            , MoveAtopAvailableControl  , SwitchingToPending , none                         , none            >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < SwitchingToPending , none                      , Pending            , EnterControl                 , none            >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < Pending            , MoveAtopAvailableControl  , SwitchingToPending , LeaveControl                 , none            >,
    Row < Pending            , MoveAtopBusyControl       , SwitchingToIgnored , LeaveControl                 , none            >,
    Row < Pending            , MoveAtopNothing           , Idle               , LeaveControl                 , none            >,
    Row < Pending            , ControlBecameDisabled     , Idle               , LeaveControl                 , none            >,
    Row < Pending            , Move                      , Pending            , NotifyMove                   , none            >,
    Row < Pending            , Down                      , Engaged            , AS2<SaveEngaged, NotifyDown> , none            >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < Engaged            , Up                        , Pending            , NotifyUp                     , none            >,
    Row < Engaged            , MoveAtopAvailableControl  , EngagedRemotely    , NotifyEngagedEscape          , none            >,
    Row < Engaged            , MoveAtopBusyControl       , EngagedRemotely    , NotifyEngagedEscape          , none            >,
    Row < Engaged            , MoveAtopNothing           , EngagedRemotely    , NotifyEngagedEscape          , none            >,
    Row < Engaged            , Move                      , Engaged            , NotifyMove                   , none            >,
    Row < Engaged            , ControlBecameDisabled     , Idle               , LeaveControl                 , none            >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < EngagedRemotely    , MoveAtopEngagedControl    , Engaged            , NotifyEngagedReturn          , none            >,
    Row < EngagedRemotely    , Up                        , Idle               , AS2<NotifyUp, LeaveControl>  , IsAtopNothing   >,
    Row < EngagedRemotely    , Up                        , SwitchingToIgnored , AS2<NotifyUp, LeaveControl>  , IsAtopBusy      >,
    Row < EngagedRemotely    , Up                        , SwitchingToPending , AS2<NotifyUp, LeaveControl>  , IsAtopAvailable >,
    Row < EngagedRemotely    , Move                      , EngagedRemotely    , NotifyMove                   , none            >,
    Row < EngagedRemotely    , ControlBecameDisabled     , Idle               , LeaveControl                 , none            >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < SwitchingToIgnored , none                      , HasBusy            , none                         , none            >
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    > {};

    // @formatter:on

private:
    Input* _parent;

};

typedef state_machine<PointerStateMachineFrontEnd> PointerStateMachine;

class TouchStateMachineFrontEnd: public state_machine_def<TouchStateMachineFrontEnd>
{

public:
    TouchStateMachineFrontEnd(Input* parent)
        : _parent(parent)
    {
    }

    // states
    struct Idle: public state<>
    {
        template<class Event, class Fsm>
        void on_entry(Event const& evt, Fsm& fsm)
        {
            fsm._parent->SetIsActive(false);
        }

        template<class Event, class Fsm>
        void on_exit(Event const& evt, Fsm& fsm)
        {
            fsm._parent->SetIsActive(true);
        }
    };
    struct Engaged: public state<>
    {
    };
    struct EngagedRemotely: public state<>
    {
    };
    struct Ignored: public state<>
    {
    };

    // guards
    struct IsAtopBusy
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            return fsm._parent->IsAtopBusy();
        }
    };
    struct IsAtopAvailable
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            return fsm._parent->IsAtopAvailable();
        }
    };
    struct IsAtopNothing
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            return fsm._parent->IsAtopNothing();
        }
    };

    // actions
    struct EnterControl
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->EnterControl();
        }
    };
    struct LeaveControl
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->LeaveControl();
        }
    };
    struct SaveEngaged
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->SaveEngaged();
        }
    };

    struct NotifyMove
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->SendNotifyMove();
        }
    };
    struct NotifyDown
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->SendNotifyDown();
        }
    };
    struct NotifyUp
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->SendNotifyUp();
        }
    };
    struct NotifyEngagedEscape
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->SendNotifyEngagedEscape();
        }
    };
    struct NotifyEngagedReturn
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            fsm._parent->SendNotifyEngagedReturn();
        }
    };

    // Replaces the default no-transition response.
    template<class FSM, class Event>
    void no_transition(Event const& e, FSM&, int state)
    {
        // Simply ignore any event that doesn't generate a transition
    }

    // Set up the starting state of the state machine
    typedef Idle initial_state;


    // Transition table for state machine
    // @formatter:off


    // NOTE: Idle must be the first state listed in this table
    struct transition_table : boost::mpl::vector<
    //    Start                Event                       Next State           Action                         Guard
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < Idle               , Down                      , Engaged            , AS3<EnterControl
                                                                                   ,SaveEngaged
                                                                                   ,NotifyDown>              , IsAtopAvailable   >,
    Row < Idle               , Down                      , Ignored            , none                         , Or_<IsAtopBusy,
                                                                                                                   IsAtopNothing>>,
    //  +--------------------+---------------------------+------------------  +------------------------------+-------------------+
    Row < Ignored            , Up                        , Idle               , none                         , none              >,
    Row < Ignored            , ControlBecameDisabled     , Idle               , none                         , none              >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-------------------+
    Row < Engaged            , Up                        , Idle               , AS2<NotifyUp, LeaveControl>  , none              >,
    Row < Engaged            , MoveAtopAvailableControl  , EngagedRemotely    , NotifyEngagedEscape          , none              >,
    Row < Engaged            , MoveAtopBusyControl       , EngagedRemotely    , NotifyEngagedEscape          , none              >,
    Row < Engaged            , MoveAtopNothing           , EngagedRemotely    , NotifyEngagedEscape          , none              >,
    Row < Engaged            , Move                      , Engaged            , NotifyMove                   , none              >,
    Row < Engaged            , ControlBecameDisabled     , Idle               , LeaveControl                 , none              >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-------------------+
    Row < EngagedRemotely    , MoveAtopEngagedControl    , Engaged            , NotifyEngagedReturn          , none              >,
    Row < EngagedRemotely    , Up                        , Idle               , AS2<NotifyUp, LeaveControl>  , none              >,
    Row < EngagedRemotely    , Move                      , EngagedRemotely    , NotifyMove                   , none              >,
    Row < EngagedRemotely    , ControlBecameDisabled     , Idle               , LeaveControl                 , none              >
    //  +--------------------+---------------------------+------------------  +------------------------------+-------------------+
    > {};

    // @formatter:on

private:
    Input* _parent;

};

typedef state_machine<TouchStateMachineFrontEnd> TouchStateMachine;
}

Input::Input(const InputId& inputId)
    : _inputId(inputId),
      _atopControl(nullptr),
      _target(nullptr),
      _atopElementInfo({}),
      _targetEnabledState(false),
      _isDown(false),
      _isDebugLoggingEnabled(false),
      _isActive(false)
{
    if (_inputId.IsPointer())
    {
        _inputType = InputType::Pointer;

        // Create state machine
        auto stateMachine = new SmInput::PointerStateMachine(this);
        stateMachine->start();
        _stateMachine = stateMachine;
    }
    else
    {
        _inputType = InputType::Touch;

        // Create state machine
        auto stateMachine = new SmInput::TouchStateMachine(this);
        stateMachine->start();
        _stateMachine = stateMachine;
    }

}

Input::~Input()
{
    // Delete state machine
    if (InputType::Pointer == _inputType)
    {
        auto stateMachine = (SmInput::PointerStateMachine*) _stateMachine;
        delete stateMachine;
    }
    else
    {
        auto stateMachine = (SmInput::TouchStateMachine*) _stateMachine;
        delete stateMachine;
    }
    _stateMachine = nullptr;
}

bool Input::NotifyNewPoint(Point point, ElementQueryInfo elementQueryInfo)
{
    _shouldUpdateScreen = false;

    _point = point;

    _atopElementInfo = elementQueryInfo;

    if (_atopControl != elementQueryInfo.ElementAtPoint)
    {
        // We need to update the atop control
        _atopControl = dynamic_cast<Control*>(elementQueryInfo.ElementAtPoint);
    }

    CheckTargetEnabledStatus();

    ProcessEvent(SmInput::Move());

    if (_isDebugLoggingEnabled)
    {
        _debugLogEntries.push_back(LogEntry{_point, _isActive});
    }

    return _shouldUpdateScreen;
}

bool Input::NotifyDown()
{
    _shouldUpdateScreen = false;

    _isDown = true;

    CheckTargetEnabledStatus();

    ProcessEvent(SmInput::Down());

    return _shouldUpdateScreen;
}

bool Input::NotifyUp()
{
    _shouldUpdateScreen = false;

    _isDown = false;

    CheckTargetEnabledStatus();

    ProcessEvent(SmInput::Up());

    return _shouldUpdateScreen;
}

bool Input::IsAtopControl()
{
    return bool(_atopControl);
}

bool Input::IsAtopTarget()
{
    return _target && (_atopControl == _target);
}

void Input::SetTargetToAtopControl()
{
    _target = _atopControl;

    // We need to check this now since the target has just changed
    _targetEnabledState = CheckTargetEnabledStatusHelper();
}

void Input::SetTargetToNothing()
{
    _target = nullptr;
}

void Input::CheckTargetEnabledStatus()
{
    if (!_target)
    {
        return;
    }

    bool isEnabled = CheckTargetEnabledStatusHelper();

    if (isEnabled != _targetEnabledState)
    {
        // The state has changed since the last time we checked
        if (isEnabled)
        {
            ProcessEvent(SmInput::TargetBecameEnabled());
        }
        else
        {
            ProcessEvent(SmInput::TargetBecameDisabled());
        }
    }
}

bool Input::CheckTargetEnabledStatusHelper() const
{
    auto isDisabled = _target->ThisOrAncestors([](Element* e)
                                               { return !e->GetIsEnabled(); });

    auto isEnabled = !isDisabled;
    return isEnabled;
}

bool Input::TargetIsEnabled()
{
    return _targetEnabledState;
}

bool Input::TargetIsBusy()
{
    return _target->HasActiveInput();
}

void Input::SendNotifyBusy()
{
    // Lock this control so it is not used by another input
    _target->SetHasActiveInput(true);
}

void Input::SendNotifyAvailable()
{
    // Free this control to be used by another input
    _target->SetHasActiveInput(false);
}

void Input::SendNotifyEnter()
{
    // Notify the control that we've entered
    InputAction inputAction;
    if (InputType::Touch == _inputType)
    {
        // For touch inputs we always send EnterReleased since EnterPushed has a specific Pointer
        // meaning which doesn't apply to touch.
        inputAction = InputAction::EnterReleased;
    }
    else
    {
        // For the Pointer input we distinguish between entering already pushed
        // or entering released.
        if (_isDown)
        {
            inputAction = InputAction::EnterPushed;
        }
        else
        {
            inputAction = InputAction::EnterReleased;
        }
    }

    bool shouldUpdateScreen;
    _target->NotifyInput(_inputType, inputAction, _point, shouldUpdateScreen);

    if (shouldUpdateScreen)
    {
        _shouldUpdateScreen = true;
    }
}

void Input::SendNotifyLeave()
{

    // Notify the control that we've left
    bool shouldUpdateScreen;
    _target->NotifyInput(_inputType, InputAction::Leave, _point, shouldUpdateScreen);
    if (shouldUpdateScreen)
    {
        _shouldUpdateScreen = true;
    }
}

void Input::SendNotifyMove()
{
    bool shouldUpdateScreen;
    _target->NotifyInput(_inputType, InputAction::Move, _point, shouldUpdateScreen);

    if (shouldUpdateScreen)
    {
        _shouldUpdateScreen = true;
    }
}

void Input::SendNotifyDown()
{
    bool shouldUpdateScreen;
    _target->NotifyInput(_inputType, InputAction::Push, _point, shouldUpdateScreen);

    if (shouldUpdateScreen)
    {
        _shouldUpdateScreen = true;
    }
}

void Input::SendNotifyUp()
{
    bool shouldUpdateScreen;
    _target->NotifyInput(_inputType, InputAction::Release, _point, shouldUpdateScreen);

    if (shouldUpdateScreen)
    {
        _shouldUpdateScreen = true;
    }
}

void Input::SendNotifyEngagedEscape()
{
    bool shouldUpdateScreen;
    _target->NotifyInput(_inputType, InputAction::EngagedEscape, _point, shouldUpdateScreen);

    if (shouldUpdateScreen)
    {
        _shouldUpdateScreen = true;
    }
}

void Input::SendNotifyEngagedReturn()
{
    bool shouldUpdateScreen;
    _target->NotifyInput(_inputType, InputAction::EngagedReturn, _point, shouldUpdateScreen);

    if (shouldUpdateScreen)
    {
        _shouldUpdateScreen = true;
    }
}

template<class Event>
void Input::ProcessEvent(const Event& evt)
{
    if (InputType::Pointer == _inputType)
    {
        ((SmInput::PointerStateMachine*) _stateMachine)->process_event(evt);
    }
    else
    {
        ((SmInput::TouchStateMachine*) _stateMachine)->process_event(evt);
    }
}

const Point& Input::GetPoint() const
{
    return _point;
}

const InputType& Input::GetInputType() const
{
    return _inputType;
}

bool Input::GetIsActive() const
{
    return _isActive;
}

void Input::EnableDebugLogging()
{
    _isDebugLoggingEnabled = true;
}

std::list<Input::LogEntry> Input::GetRecentDebugLogEntries()
{
    auto listToReturn = _debugLogEntries;

    // Clear the list for next time
    _debugLogEntries.clear();

    // Return the stored log entries since last time it was requested
    return listToReturn;
}

void Input::SetIsActive(bool isActive)
{
    _isActive = isActive;

    if (_isDebugLoggingEnabled)
    {
        _debugLogEntries.push_back(LogEntry{_point, isActive});
    }
}
}