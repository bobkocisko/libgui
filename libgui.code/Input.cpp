//
// Created by Bob Kocisko on 12/18/15.
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
struct TargetBecameInactive
{
};
struct TargetBecameActive
{
};

class StateMachineFrontEnd: public state_machine_def<StateMachineFrontEnd>
{

#ifdef DBG
#define DBG_PRINT(x) printf(x "\n"); fflush(stdout);
#define DBG_ENTER_EXIT(x) \
template<class Event, class Fsm> \
void on_entry(Event const& evt, Fsm& fsm) \
{ \
    DBG_PRINT("Enter " x) \
} \
\
template<class Event, class Fsm> \
void on_exit(Event const& evt, Fsm& fsm) \
{ \
    DBG_PRINT("Exit " x) \
} \

#else
#define DBG_PRINT(x) ;
#define DBG_ENTER_EXIT(x) ;
#endif


public:

  // Tell msm not to catch exceptions, let them propagate out.  This is a
  // sticky subject and not very clear:
  // https://lists.boost.org/boost-users/2012/03/73904.php
  // So I took to the msm source code and as best as I can tell the only
  // potential problem with doing this is ending up with inconsistent current
  // states for orthogonal regions (if used--we don't) if the second region has
  // an exception and thereby fails to transition.  If the first region throws
  // then neither region will transition and there's a guarantee by UML that
  // the regions handle the event in the order specified by the initial_state
  // type:
  // https://www.boost.org/doc/libs/1_55_0/libs/msm/doc/HTML/ch03s02.html
  //  "Orthogonal regions, terminate state, event deferring"
  // So basically as long as no action or entry state for the second region
  // is allowed to throw an error then everything should be consistent.
  typedef int no_exception_thrown;

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
  struct Startup: public state<>
  { DBG_ENTER_EXIT("Startup")
  };
  struct IdlePointer: public state<>
  {
    template<class Event, class Fsm>
    void on_entry(Event const& evt, Fsm& fsm)
    {
      DBG_PRINT("Enter IdlePointer")
      fsm._parent->SetIsActive(false);
    }

    template<class Event, class Fsm>
    void on_exit(Event const& evt, Fsm& fsm)
    {
      DBG_PRINT("Exit IdlePointer");
      fsm._parent->SetIsActive(true);
    }
  };
  struct IdleTouch: public state<>
  {
    template<class Event, class Fsm>
    void on_entry(Event const& evt, Fsm& fsm)
    {
      DBG_PRINT("Enter IdleTouch")
      fsm._parent->SetIsActive(false);
    }

    template<class Event, class Fsm>
    void on_exit(Event const& evt, Fsm& fsm)
    {
      DBG_PRINT("Exit IdleTouch");
      fsm._parent->SetIsActive(true);
    }
  };
  struct RetargetPointer: public state<>
  { DBG_ENTER_EXIT("RetargetPointer")
  };
  struct HasTarget_: public state_machine_def<HasTarget_>
  {
    typedef int no_exception_thrown;

    void SetParent(Input* parent)
    {
      _parent = parent;
    }

    template<class Event, class Fsm>
    void on_entry(Event const& evt, Fsm& fsm)
    {
      DBG_PRINT("Enter HasTarget")
      fsm._parent->SetTargetToAtopControl();
    }

    template<class Event, class Fsm>
    void on_exit(Event const& evt, Fsm& fsm)
    {
      DBG_PRINT("Exit HasTarget")
      fsm._parent->SetTargetToNothing();
    }

    // states
    struct DecideTargetIsActive: public state<>
    { DBG_ENTER_EXIT("DecideTargetIsActive")
    };
    struct HasInactive: public state<>
    { DBG_ENTER_EXIT("HasInactive")
    };
    struct HasActive_: public state_machine_def<HasActive_>
    {
      typedef int no_exception_thrown;

      void SetParent(Input* parent)
      {
        _parent = parent;
      }

      // states
      struct DecideTargetIsBusy: public state<>
      { DBG_ENTER_EXIT("DecideTargetIsBusy")
      };
      struct HasBusy: public state<>
      { DBG_ENTER_EXIT("HasBusy")
      };

      struct HasAvailable_: public state_machine_def<HasAvailable_>
      {
        typedef int no_exception_thrown;

        void SetParent(Input* parent)
        {
          _parent = parent;
        }

        template<class Event, class Fsm>
        void on_entry(Event const& evt, Fsm& fsm)
        {
          DBG_PRINT("Enter HasAvailable")
          fsm._parent->SendNotifyBusy();
          fsm._parent->SendNotifyEnter();
        }

        template<class Event, class Fsm>
        void on_exit(Event const& evt, Fsm& fsm)
        {
          DBG_PRINT("Exit HasAvailable")
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
        { DBG_ENTER_EXIT("DecideEventType")
        };
        struct Pending: public state<>
        { DBG_ENTER_EXIT("Pending")
        };
        struct Engaged: public state<>
        { DBG_ENTER_EXIT("Engaged")
        };
        struct EngagedRemotely: public state<>
        { DBG_ENTER_EXIT("EngagedRemotely")
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
    typedef back::state_machine<HasActive_> HasActive;

    // guards
    struct TargetIsActive
    {
      template<class EVT, class FSM, class SourceState, class TargetState>
      bool operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
      {
        return fsm._parent->TargetIsActive();
      }
    };

    // Replaces the default no-transition response.
    template<class FSM, class Event>
    void no_transition(Event const& e, FSM&, int state)
    {
      // Simply ignore any event that doesn't generate a transition
    }

    // Set up the starting state of the state machine
    typedef DecideTargetIsActive initial_state;

    // Transition table for state machine
    // @formatter:off

        struct transition_table : boost::mpl::vector<
        //    Start                    Event                   Next State      Action    Guard
        //  +------------------------+-----------------------+---------------+---------+------------------------+
        Row < DecideTargetIsActive   , none                  , HasInactive   , none    , Not_<TargetIsActive>   >,
        Row < DecideTargetIsActive   , none                  , HasActive     , none    , TargetIsActive         >,
        //  +------------------------+-----------------------+---------------+---------+------------------------+
        Row < HasInactive            , TargetBecameActive    , HasActive     , none    , none                   >,
        //  +------------------------+-----------------------+---------------+---------+------------------------+
        Row < HasActive              , TargetBecameInactive  , HasInactive   , none    , IsAtopTarget           >
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
  typedef Startup initial_state;


  // Transition table for state machine
  // @formatter:off


    // NOTE: Startup must be the first state listed in this table
    struct transition_table : boost::mpl::vector<
    //    Start             Event          Next State         Action   Guard
    //  +-----------------+--------------+------------------+--------+------------------------------------+
    Row < Startup         , boost::any   , IdlePointer      , none   , IsPointer                          >,
    Row < Startup         , boost::any   , IdleTouch        , none   , IsTouch                            >,
    //  +-----------------+--------------+------------------+--------+------------------------------------+
    Row < IdlePointer     , boost::any   , HasTarget        , none   , And_<Not_<EventTypeIsAnonymous>,
                                                                            IsAtopControl>                >,
    //  +-----------------+--------------+------------------+--------+------------------------------------+
    Row < IdleTouch       , Down         , HasTarget        , none   , IsAtopControl                      >,
    //  +-----------------+--------------+------------------+--------+------------------------------------+
    Row < HasTarget       , boost::any   , RetargetPointer  , none   , And_<IsPointer,
                                                                        And_<Not_<EventTypeIsAnonymous>,
                                                                             Not_<IsAtopTarget>>>         >,
    //  +-----------------+--------------+------------------+--------+------------------------------------+
    Row < HasTarget       , boost::any   , IdleTouch        , none   , And_<IsTouch,
                                                                            Not_<EventTypeIsAnonymous>>   >,
    //  +-----------------+--------------+------------------+--------+------------------------------------+
    Row < RetargetPointer , none         , IdlePointer      , none   , Not_<IsAtopControl>                >,
    Row < RetargetPointer , none         , HasTarget        , none   , IsAtopControl                      >
    //  +-----------------+--------------+------------------+--------+------------------------------------+
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
    _targetActiveState(false),
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

  // Initialize the point to -1, -1 to satisfy the api of ElementManager's GetCurrentPoint method
  _point = {-1, -1};

  // Create state machine
  auto stateMachine = new SmInput::StateMachine(this);

  // Set submachine links back to the parent class instance
  auto& hasTarget = stateMachine->get_state<SmInput::StateMachineFrontEnd::HasTarget&>();
  hasTarget.SetParent(this);
  auto& hasActive = hasTarget.get_state<SmInput::StateMachineFrontEnd::HasTarget_::HasActive&>();
  hasActive.SetParent(this);
  auto& hasAvailable = hasActive.get_state<SmInput::StateMachineFrontEnd::HasTarget_::HasActive_::HasAvailable&>();
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

void Input::NotifyNewPoint(Point point, ElementQueryInfo elementQueryInfo)
{
  _point = point;

  _atopElementInfo = elementQueryInfo;

  if (_atopControl != elementQueryInfo.ElementAtPoint)
  {
    // We need to update the atop control
    _atopControl = dynamic_cast<Control*>(elementQueryInfo.ElementAtPoint);
  }

  CheckTargetActiveStatus();

  ProcessEvent(SmInput::Move());

  if (_isDebugLoggingEnabled)
  {
    _debugLogEntries.push_back(LogEntry{_point, _isActive});
  }
}

void Input::NotifyDown()
{
  _isDown = true;

  CheckTargetActiveStatus();

  ProcessEvent(SmInput::Down());
}

void Input::NotifyUp()
{
  _isDown = false;

  CheckTargetActiveStatus();

  ProcessEvent(SmInput::Up());
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
  _targetActiveState = CheckTargetActiveStatusHelper();
}

void Input::SetTargetToNothing()
{
  _target = nullptr;
}

void Input::CheckTargetActiveStatus()
{
  if (!_target)
  {
    return;
  }

  bool isActive = CheckTargetActiveStatusHelper();

  if (isActive != _targetActiveState)
  {
    // The state has changed since the last time we checked
    _targetActiveState = isActive;

    if (isActive)
    {
      ProcessEvent(SmInput::TargetBecameActive());
    }
    else
    {
      ProcessEvent(SmInput::TargetBecameInactive());
    }
  }
}

bool Input::CheckTargetActiveStatusHelper() const
{
  auto isInactive = _target->ThisOrAncestors([](Element* e) {
    return !e->GetIsEnabled() ||
           !e->GetIsVisible();
  });

  auto isActive = !isInactive;
  return isActive;
}

bool Input::TargetIsActive()
{
  return _targetActiveState;
}

bool Input::TargetIsBusy()
{
  if (!_target)
  {
    return false;
  }

  return _target->HasActiveInput();
}

void Input::SendNotifyBusy()
{
  if (!_target)
  {
    return;
  }

  // Lock this control so it is not used by another input
  _target->SetHasActiveInput(true);
}

void Input::SendNotifyAvailable()
{
  if (!_target)
  {
    return;
  }

  // Free this control to be used by another input
  _target->SetHasActiveInput(false);
}

void Input::SendNotifyEnter()
{
  if (!_target)
  {
    return;
  }

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

  _target->NotifyInput(_inputType, inputAction, _point);
}

void Input::SendNotifyLeave()
{
  if (!_target)
  {
    return;
  }

  // Notify the control that we've left
  _target->NotifyInput(_inputType, InputAction::Leave, _point);
}

void Input::SendNotifyMove()
{
  if (!_target)
  {
    return;
  }

  _target->NotifyInput(_inputType, InputAction::Move, _point);
}

void Input::SendNotifyDown()
{
  if (!_target)
  {
    return;
  }

  _target->NotifyInput(_inputType, InputAction::Push, _point);
}

void Input::SendNotifyUp()
{
  if (!_target)
  {
    return;
  }

  _target->NotifyInput(_inputType, InputAction::Release, _point);
}

void Input::SendNotifyEngagedEscape()
{
  if (!_target)
  {
    return;
  }

  _target->NotifyInput(_inputType, InputAction::EngagedEscape, _point);
}

void Input::SendNotifyEngagedReturn()
{
  if (!_target)
  {
    return;
  }

  _target->NotifyInput(_inputType, InputAction::EngagedReturn, _point);
}

template<class Event>
void Input::ProcessEvent(const Event& evt)
{
#ifdef DBG
  printf("\nProcessEvent %s...\n", typeid(evt).name());
  fflush(stdout);
#endif

  // Enable reading the active event while processing
  _activeEvent = evt;
  ScopeExit onScopeExit(
    [this]() {
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

bool Input::GetIsDown() const
{
  return _isDown;
}

void Input::NotifyControlIsBeingDestroyed(Control* control)
{
  // Prevent SEGFAULTs by ensuring that we do not
  // keep references to destroyed controls.
  if (_atopControl == control)
  {
    _atopControl = nullptr;
  }

  if (_target == control)
  {
    _target = nullptr;
  }
}
}