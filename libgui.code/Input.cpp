//
// Created by sjm-kociskobd on 12/18/15.
//

#include "libgui/Input.h"

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
struct MoveAtopNothing
{
};
struct MoveAtopAvailableControl
{
};
struct MoveAtopBusyControl
{
};
struct MoveAtopEngagedControl
{
};
struct Move // Moving that doesn't cause any of the previous events
{
};
struct Up
{
};
struct Down
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
    struct Ignored: public state<>
    {
    };
    struct SwitchingToPending: public state<>
    {
    };
    struct SwitchingToIgnored: public state<>
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


    // NOTE: The order of the states listed in this table must match the order in the State enum
    struct transition_table : boost::mpl::vector<
    //    Start                Event                       Next State           Action                         Guard
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < Idle               , MoveAtopAvailableControl  , Pending            , EnterControl                 , none            >,
    Row < Idle               , MoveAtopBusyControl       , Ignored            , none                         , none            >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < Ignored            , MoveAtopNothing           , Idle               , none                         , none            >,
    Row < Ignored            , MoveAtopAvailableControl  , SwitchingToPending , none                         , none            >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < SwitchingToPending , none                      , Pending            , EnterControl                 , none            >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < Pending            , MoveAtopAvailableControl  , SwitchingToPending , LeaveControl                 , none            >,
    Row < Pending            , MoveAtopBusyControl       , SwitchingToIgnored , LeaveControl                 , none            >,
    Row < Pending            , MoveAtopNothing           , Idle               , LeaveControl                 , none            >,
    Row < Pending            , Move                      , Pending            , NotifyMove                   , none            >,
    Row < Pending            , Down                      , Engaged            , AS2<SaveEngaged, NotifyDown> , none            >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < Engaged            , Up                        , Pending            , NotifyUp                     , none            >,
    Row < Engaged            , MoveAtopAvailableControl  , EngagedRemotely    , NotifyEngagedEscape          , none            >,
    Row < Engaged            , MoveAtopBusyControl       , EngagedRemotely    , NotifyEngagedEscape          , none            >,
    Row < Engaged            , MoveAtopNothing           , EngagedRemotely    , NotifyEngagedEscape          , none            >,
    Row < Engaged            , Move                      , Engaged            , NotifyMove                   , none            >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < EngagedRemotely    , MoveAtopEngagedControl    , Engaged            , NotifyEngagedReturn          , none            >,
    Row < EngagedRemotely    , Up                        , Idle               , AS2<NotifyUp, LeaveControl>  , IsAtopNothing   >,
    Row < EngagedRemotely    , Up                        , SwitchingToIgnored , AS2<NotifyUp, LeaveControl>  , IsAtopBusy      >,
    Row < EngagedRemotely    , Up                        , SwitchingToPending , AS2<NotifyUp, LeaveControl>  , IsAtopAvailable >,
    Row < EngagedRemotely    , Move                      , EngagedRemotely    , NotifyMove                   , none            >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-----------------+
    Row < SwitchingToIgnored , none                      , Ignored            , none                         , none            >
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


    // NOTE: The order of the states listed in this table must match the order in the State enum
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
    //  +--------------------+---------------------------+------------------  +------------------------------+-------------------+
    Row < Engaged            , Up                        , Idle               , AS2<NotifyUp, LeaveControl>  , none              >,
    Row < Engaged            , MoveAtopAvailableControl  , EngagedRemotely    , NotifyEngagedEscape          , none              >,
    Row < Engaged            , MoveAtopBusyControl       , EngagedRemotely    , NotifyEngagedEscape          , none              >,
    Row < Engaged            , MoveAtopNothing           , EngagedRemotely    , NotifyEngagedEscape          , none              >,
    Row < Engaged            , Move                      , Engaged            , NotifyMove                   , none              >,
    //  +--------------------+---------------------------+------------------  +------------------------------+-------------------+
    Row < EngagedRemotely    , MoveAtopEngagedControl    , Engaged            , NotifyEngagedReturn          , none              >,
    Row < EngagedRemotely    , Up                        , Idle               , AS2<NotifyUp, LeaveControl>  , none              >,
    Row < EngagedRemotely    , Move                      , EngagedRemotely    , NotifyMove                   , none              >
    //  +--------------------+---------------------------+------------------  +------------------------------+-------------------+
    > {};

    // @formatter:on

private:
    Input* _parent;

};

typedef state_machine<TouchStateMachineFrontEnd> TouchStateMachine;
}

Input::Input(const InputId& inputId)
    : _inputId(inputId)
{
    _isDown                 = false;
    _atopElement            = nullptr;
    _activeControl          = nullptr;
    _isActiveControlEngaged = false;

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

bool Input::NotifyNewPoint(Point point, Element* atopElement)
{
    _shouldUpdateScreen = false;

    _point = point;

    if (atopElement != _atopElement)
    {
        // We've changed to be atop a new element
        _atopElement = atopElement;

        auto atopNewControl = GetAtopEnabledControl();
        if (atopNewControl)
        {
            if (_activeControl == atopNewControl)
            {
                if (_isActiveControlEngaged)
                {
                    ProcessEvent(SmInput::MoveAtopEngagedControl());
                }
            }
            else if (atopNewControl->HasActiveInput())
            {
                ProcessEvent(SmInput::MoveAtopBusyControl());
            }
            else
            {
                ProcessEvent(SmInput::MoveAtopAvailableControl());
            }
        }
        else
        {
            ProcessEvent(SmInput::MoveAtopNothing());
        }
    }
    else
    {
        // The atop element hasn't changed
        ProcessEvent(SmInput::Move());
    }

    return _shouldUpdateScreen;
}

bool Input::NotifyDown()
{
    _shouldUpdateScreen = false;

    _isDown = true;

    ProcessEvent(SmInput::Down());

    return _shouldUpdateScreen;
}

bool Input::NotifyUp()
{
    _shouldUpdateScreen = false;

    _isDown = false;

    ProcessEvent(SmInput::Up());

    return _shouldUpdateScreen;
}

Control* Input::GetAtopEnabledControl()
{
    auto atopControl = dynamic_cast<Control*>(_atopElement);
    if (atopControl && atopControl->GetIsEnabled())
    {
        return atopControl;
    }
    else
    {
        return nullptr;
    }
}

bool Input::IsAtopBusy()
{
    auto atopControl = GetAtopEnabledControl();
    return (atopControl && atopControl->HasActiveInput());
}

bool Input::IsAtopAvailable()
{
    auto atopControl = GetAtopEnabledControl();
    return (atopControl && !atopControl->HasActiveInput());
}

bool Input::IsAtopNothing()
{
    auto atopControl = GetAtopEnabledControl();
    return (nullptr == atopControl);
}

void Input::EnterControl()
{
    auto atopControl = GetAtopEnabledControl();
    _activeControl          = atopControl;
    _isActiveControlEngaged = false;

    // Lock this control so it is not used by another input
    _activeControl->SetHasActiveInput(true);

    // Notify the control that we've entered
    InputAction inputAction;
    if (_isDown)
    {
        inputAction = InputAction::EnterPushed;
    }
    else
    {
        inputAction = InputAction::EnterReleased;
    }

    bool shouldUpdateScreen;
    _activeControl->NotifyInput(_inputType, inputAction, _point, shouldUpdateScreen);

    if (shouldUpdateScreen)
    {
        _shouldUpdateScreen = true;
    }
}

void Input::LeaveControl()
{

    // Notify the control that we've left
    bool shouldUpdateScreen;
    _activeControl->NotifyInput(_inputType, InputAction::Leave, _point, shouldUpdateScreen);
    if (shouldUpdateScreen)
    {
        _shouldUpdateScreen = true;
    }

    // Free this control to be used by another input
    _activeControl->SetHasActiveInput(false);

    _activeControl          = nullptr;
    _isActiveControlEngaged = false;
}

void Input::SaveEngaged()
{
    _isActiveControlEngaged = true;
}

void Input::SendNotifyMove()
{
    bool shouldUpdateScreen;
    _activeControl->NotifyInput(_inputType, InputAction::Move, _point, shouldUpdateScreen);

    if (shouldUpdateScreen)
    {
        _shouldUpdateScreen = true;
    }
}

void Input::SendNotifyDown()
{
    bool shouldUpdateScreen;
    _activeControl->NotifyInput(_inputType, InputAction::Push, _point, shouldUpdateScreen);

    if (shouldUpdateScreen)
    {
        _shouldUpdateScreen = true;
    }
}

void Input::SendNotifyUp()
{
    bool shouldUpdateScreen;
    _activeControl->NotifyInput(_inputType, InputAction::Release, _point, shouldUpdateScreen);

    if (shouldUpdateScreen)
    {
        _shouldUpdateScreen = true;
    }
}

void Input::SendNotifyEngagedEscape()
{
    bool shouldUpdateScreen;
    _activeControl->NotifyInput(_inputType, InputAction::EngagedEscape, _point, shouldUpdateScreen);

    if (shouldUpdateScreen)
    {
        _shouldUpdateScreen = true;
    }
}

void Input::SendNotifyEngagedReturn()
{
    bool shouldUpdateScreen;
    _activeControl->NotifyInput(_inputType, InputAction::EngagedReturn, _point, shouldUpdateScreen);

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
}