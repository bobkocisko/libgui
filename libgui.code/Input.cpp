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
#include <type_traits>
#include <libgui/ScopeExit.h>

using namespace boost::msm::front;
namespace back = boost::msm::back;
using boost::msm::front::euml::func_state;
using emptyvector = boost::fusion::vector<>;
using boost::mpl::vector1;
using boost::mpl::vector2;
using boost::mpl::vector3;
using boost::msm::TerminateFlag;
using boost::msm::front::euml::Or_;
using boost::msm::front::euml::Not_;
using boost::msm::front::euml::And_;

template<class A1, class A2>
using AS2 = ActionSequence_<vector2<A1, A2>>;

template<class A1, class A2, class A3>
using AS3 = ActionSequence_<vector3<A1, A2, A3>>;

namespace libgui
{

namespace SmInput
{
#define DBG(x) printf(x "\n"); fflush(stdout);

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

class StateMachineFrontEnd: public state_machine_def<StateMachineFrontEnd>
{

public:
    StateMachineFrontEnd(Input* parent)
        : _parent(parent)
    {
    }

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
    struct IsPointer
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            return fsm._parent->IsPointer();
        }
    };
    struct IsTouch
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            return fsm._parent->IsTouch();
        }
    };
    struct EventTypeIsAnonymous
    {
        template<class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
        {
            if (!std::is_same<EVT, boost::any>::value)
            {
                return false;
            }

            auto& any = (boost::any&) evt;
            return any.type() == typeid(none);
        }
    };

    // states
    struct Idle: public state<>
    {
        template<class Event, class Fsm>
        void on_entry(Event const& evt, Fsm& fsm)
        {
            DBG("Enter Idle")
            fsm._parent->SetIsActive(false);
        }

        template<class Event, class Fsm>
        void on_exit(Event const& evt, Fsm& fsm)
        {
            DBG("Exit Idle")
            fsm._parent->SetIsActive(true);
        }
    };
    struct Retarget: public state<>
    {
        template<class Event, class Fsm>
        void on_entry(Event const& evt, Fsm& fsm)
        {
            DBG("Enter Retarget")
        }

        template<class Event, class Fsm>
        void on_exit(Event const& evt, Fsm& fsm)
        {
            DBG("Exit Retarget")
        }
    };
    struct HasTarget_: public state_machine_def<HasTarget_>
    {
        void SetParent(Input* parent)
        {
            _parent = parent;
        }

        template<class Event, class Fsm>
        void on_entry(Event const& evt, Fsm& fsm)
        {
            DBG("Enter HasTarget")
            fsm._parent->SetTargetToAtopControl();
        }

        template<class Event, class Fsm>
        void on_exit(Event const& evt, Fsm& fsm)
        {
            DBG("Exit HasTarget")
            fsm._parent->SetTargetToNothing();
        }

        // states
        struct DecideTargetIsEnabled: public state<>
        {
            template<class Event, class Fsm>
            void on_entry(Event const& evt, Fsm& fsm)
            {
                DBG("Enter DecideTargetIsEnabled")
            }

            template<class Event, class Fsm>
            void on_exit(Event const& evt, Fsm& fsm)
            {
                DBG("Exit DecideTargetIsEnabled")
            }

        };
        struct HasDisabled: public state<>
        {
            template<class Event, class Fsm>
            void on_entry(Event const& evt, Fsm& fsm)
            {
                DBG("Enter HasDisabled")
            }

            template<class Event, class Fsm>
            void on_exit(Event const& evt, Fsm& fsm)
            {
                DBG("Exit HasDisabled")
            }
        };
        struct HasEnabled_: public state_machine_def<HasEnabled_>
        {
            void SetParent(Input* parent)
            {
                _parent = parent;
            }

            // states
            struct DecideTargetIsBusy: public state<>
            {
                template<class Event, class Fsm>
                void on_entry(Event const& evt, Fsm& fsm)
                {
                    DBG("Enter DecideTargetIsBusy")
                }

                template<class Event, class Fsm>
                void on_exit(Event const& evt, Fsm& fsm)
                {
                    DBG("Exit DecideTargetIsBusy")
                }
            };
            struct HasBusy: public state<>
            {
                template<class Event, class Fsm>
                void on_entry(Event const& evt, Fsm& fsm)
                {
                    DBG("Enter HasBusy")
                }

                template<class Event, class Fsm>
                void on_exit(Event const& evt, Fsm& fsm)
                {
                    DBG("Exit HasBusy")
                }
            };

            struct HasAvailable_: public state_machine_def<HasAvailable_>
            {
                void SetParent(Input* parent)
                {
                    _parent = parent;
                }

                template<class Event, class Fsm>
                void on_entry(Event const& evt, Fsm& fsm)
                {
                    DBG("Enter HasAvailable")
                    fsm._parent->SendNotifyBusy();
                    fsm._parent->SendNotifyEnter();
                }

                template<class Event, class Fsm>
                void on_exit(Event const& evt, Fsm& fsm)
                {
                    DBG("Exit HasAvailable")
                    printf("%s\n", fsm._parent->GetActiveEvent().type().name());
                    fflush(stdout);
                    if (fsm._parent->GetActiveEvent().type() == typeid(Up))
                    {
                        fsm._parent->SendNotifyUp();
                    }
                    fsm._parent->SendNotifyLeave();
                    fsm._parent->SendNotifyAvailable();
                }

                // actions
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

                // guards
                struct EventTypeIsDown
                {
                    template<class Event, class FSM, class SourceState, class TargetState>
                    bool operator()(Event const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
                    {
                        return (fsm._parent->GetActiveEvent().type() == typeid(Down));
                    }
                };

                // states
                struct DecideEventType: public state<>
                {
                    template<class Event, class Fsm>
                    void on_entry(Event const& evt, Fsm& fsm)
                    {
                        DBG("Enter DecideEventType")
                    }

                    template<class Event, class Fsm>
                    void on_exit(Event const& evt, Fsm& fsm)
                    {
                        DBG("Exit DecideEventType")
                    }
                };
                struct Pending: public state<>
                {
                    template<class Event, class Fsm>
                    void on_entry(Event const& evt, Fsm& fsm)
                    {
                        DBG("Enter Pending")
                    }

                    template<class Event, class Fsm>
                    void on_exit(Event const& evt, Fsm& fsm)
                    {
                        DBG("Exit Pending")
                    }
                };
                struct Engaged: public state<>
                {
                    template<class Event, class Fsm>
                    void on_entry(Event const& evt, Fsm& fsm)
                    {
                        DBG("Enter Engaged")
                    }

                    template<class Event, class Fsm>
                    void on_exit(Event const& evt, Fsm& fsm)
                    {
                        DBG("Exit Engaged")
                    }
                };
                struct EngagedRemotely: public state<>
                {
                    template<class Event, class Fsm>
                    void on_entry(Event const& evt, Fsm& fsm)
                    {
                        DBG("Enter EngagedRemotely")
                    }

                    template<class Event, class Fsm>
                    void on_exit(Event const& evt, Fsm& fsm)
                    {
                        DBG("Exit EngagedRemotely")
                    }
                };

                // Replaces the default no-transition response.
                template<class FSM, class Event>
                void no_transition(Event const& e, FSM&, int state)
                {
                    // Simply ignore any event that doesn't generate a transition
                }

                // Set up the starting state of the state machine
                typedef DecideEventType initial_state;

                // Transition table for state machine
                // @formatter:off

                struct transition_table : boost::mpl::vector<
                //    Start             Event     Next State         Action                  Guard
                //  +------------------+--------+------------------+-----------------------+-----------------------+
                Row < DecideEventType  , none   , Pending          , none                  , Not_<EventTypeIsDown> >,
                Row < DecideEventType  , none   , Engaged          , NotifyDown            , EventTypeIsDown       >,
                //  +------------------+--------+------------------+-----------------------+-----------------------+
                Row < Pending          , Move   , Pending          , NotifyMove            , IsAtopTarget          >,
                Row < Pending          , Down   , Engaged          , NotifyDown            , none                  >,
                //  +------------------+--------+------------------+-----------------------+-----------------------+
                Row < Engaged          , Up     , Pending          , NotifyUp              , IsPointer             >,
                Row < Engaged          , Move   , EngagedRemotely  , NotifyEngagedEscape   , Not_<IsAtopTarget>    >,
                Row < Engaged          , Move   , Engaged          , NotifyMove            , IsAtopTarget          >,
                //  +------------------+--------+------------------+-----------------------+-----------------------+
                Row < EngagedRemotely  , Move   , Engaged          , NotifyEngagedReturn   , IsAtopTarget          >,
                Row < EngagedRemotely  , Move   , EngagedRemotely  , NotifyMove            , Not_<IsAtopTarget>    >
                //  +------------------+--------+------------------+-----------------------+-----------------------+
                > {};

                // @formatter:on

            private:
                friend class IsAtopTarget;
                friend class IsPointer;
                Input* _parent;

            };
            typedef back::state_machine<HasAvailable_> HasAvailable;

            // guards
            struct TargetIsBusy
            {
                template<class EVT, class FSM, class SourceState, class TargetState>
                bool operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
                {
                    return fsm._parent->TargetIsBusy();
                }
            };

            // Replaces the default no-transition response.
            template<class FSM, class Event>
            void no_transition(Event const& e, FSM&, int state)
            {
                // Simply ignore any event that doesn't generate a transition
            }

            // Set up the starting state of the state machine
            typedef DecideTargetIsBusy initial_state;

            // Transition table for state machine
            // @formatter:off

            struct transition_table : boost::mpl::vector<
            //    Start                    Event                   Next State      Action    Guard
            //  +------------------------+-----------------------+---------------+---------+------------------------+
            Row < DecideTargetIsBusy     , none                  , HasBusy       , none    , TargetIsBusy           >,
            Row < DecideTargetIsBusy     , none                  , HasAvailable  , none    , Not_<TargetIsBusy>     >
            > {};

            // @formatter:on

        private:
            Input* _parent;

        };
        typedef back::state_machine<HasEnabled_> HasEnabled;

        // guards
        struct TargetIsEnabled
        {
            template<class EVT, class FSM, class SourceState, class TargetState>
            bool operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
            {
                return fsm._parent->TargetIsEnabled();
            }
        };

        // Replaces the default no-transition response.
        template<class FSM, class Event>
        void no_transition(Event const& e, FSM&, int state)
        {
            // Simply ignore any event that doesn't generate a transition
        }

        // Set up the starting state of the state machine
        typedef DecideTargetIsEnabled initial_state;

        // Transition table for state machine
        // @formatter:off

        struct transition_table : boost::mpl::vector<
        //    Start                    Event                   Next State      Action    Guard
        //  +------------------------+-----------------------+---------------+---------+------------------------+
        Row < DecideTargetIsEnabled  , none                  , HasDisabled   , none    , Not_<TargetIsEnabled>  >,
        Row < DecideTargetIsEnabled  , none                  , HasEnabled    , none    , TargetIsEnabled        >,
        //  +------------------------+-----------------------+---------------+---------+------------------------+
        Row < HasDisabled            , TargetBecameEnabled   , HasEnabled    , none    , none                   >,
        //  +------------------------+-----------------------+---------------+---------+------------------------+
        Row < HasEnabled             , TargetBecameDisabled  , HasDisabled   , none    , IsAtopTarget           >
        //  +------------------------+-----------------------+---------------+---------+------------------------+
        > {};

        // @formatter:on

    private:
        friend class IsAtopTarget;
        Input* _parent;

    };
    typedef back::state_machine<HasTarget_> HasTarget;

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
    //    Start           Event          Next State     Action   Guard
    //  +---------------+--------------+--------------+--------+------------------------------------+
    Row < Idle          , boost::any   , HasTarget    , none   , And_<Not_<EventTypeIsAnonymous>,
                                                                      IsAtopControl>                >,
    //  +---------------+--------------+--------------+--------+------------------------------------+
    Row < HasTarget     , boost::any   , Retarget     , none   , And_<Not_<EventTypeIsAnonymous>,
                                                                      And_<IsPointer,
                                                                           Not_<IsAtopTarget>>>     >,
    Row < HasTarget     , Move         , Retarget     , none   , And_<IsTouch,
                                                                      Not_<IsAtopTarget>>           >,
    Row < HasTarget     , Up           , Idle         , none   , IsTouch                            >,
    //  +---------------+--------------+--------------+--------+------------------------------------+
    Row < Retarget      , none         , Idle         , none   , Not_<IsAtopControl>                >,
    Row < Retarget      , none         , HasTarget    , none   , IsAtopControl                      >
    //  +---------------+--------------+--------------+--------+------------------------------------+
    > {};

    // @formatter:on

private:
    Input* _parent;

};

typedef back::state_machine<StateMachineFrontEnd> StateMachine;
}

Input::Input(const InputId& inputId)
    : _inputId(inputId),
      _atopControl(nullptr),
      _target(nullptr),
      _atopElementInfo({}),
      _targetEnabledState(false),
      _isDown(false),
      _isDebugLoggingEnabled(false),
      _isActive(false),
      _activeEvent(nullptr)
{
    if (_inputId.IsPointer())
    {
        _inputType = InputType::Pointer;
    }
    else
    {
        _inputType = InputType::Touch;
    }

    // Create state machine
    auto stateMachine = new SmInput::StateMachine(this);

    // Set submachine links back to the parent class instance
    auto& hasTarget = stateMachine->get_state<SmInput::StateMachineFrontEnd::HasTarget&>();
    hasTarget.SetParent(this);
    auto& hasEnabled = hasTarget.get_state<SmInput::StateMachineFrontEnd::HasTarget_::HasEnabled&>();
    hasEnabled.SetParent(this);
    auto& hasAvailable = hasEnabled.get_state<SmInput::StateMachineFrontEnd::HasTarget_::HasEnabled_::HasAvailable&>();
    hasAvailable.SetParent(this);

    stateMachine->start();
    _stateMachine = stateMachine;
}

Input::~Input()
{
    // Delete state machine
    auto stateMachine = (SmInput::StateMachine*) _stateMachine;
    delete stateMachine;
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

bool Input::IsPointer()
{
    return InputType::Pointer == _inputType;
}

bool Input::IsTouch()
{
    return InputType::Touch == _inputType;
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
        _targetEnabledState = isEnabled;

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
        // For touch inputs we always send EnterPushed since there's really no such
        // thing as 'hovering' with touch inputs.  However, it's possible to enter
        // a control from the outside while already pushed down.
        inputAction = InputAction::EnterPushed;
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
    // Enable reading the active event while processing
    printf("ProcessEvent: %s\n", typeid(evt).name());
    _activeEvent = evt;
    ScopeExit onScopeExit(
        [this]()
        {
            _activeEvent = nullptr;
        });

    ((SmInput::StateMachine*) _stateMachine)->process_event(evt);
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

boost::any& Input::GetActiveEvent()
{
    return _activeEvent;
}
}