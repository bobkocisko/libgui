//
// Created by Bob Kocisko on 8/20/18.
//

#include "libgui/Knob.h"

#include "libgui/ElementManager.h"
#include "libgui/InputAction.h"
#include "libgui/InputType.h"
#include "libgui/Point.h"
#include "libgui/ScopeExit.h"

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
using boost::msm::TerminateFlag;

namespace libgui
{

namespace SmKnob
{
// events
struct Enter
{
};
struct Leave
{
};
struct Push
{
};
struct Release
{
};
struct EngagedEscape
{
};
struct EngagedReturn
{
};

class StateMachineFrontEnd: public state_machine_def<StateMachineFrontEnd>
{

public:
  typedef int no_exception_thrown;

  StateMachineFrontEnd(Knob* parent)
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

  // actions
  struct RecordAnchor
  {
    template<class EVT, class FSM, class SourceState, class TargetState>
    void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
    {
      fsm._parent->RecordAnchor();
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
    //    Start              Event           Next State         Action           Guard
    //  +------------------+-----------    +------------------+----------------+---------+
    Row < Idle             , Enter         , Pending          , none           , none    >,
    //  +------------------+-----------    +------------------+----------------+---------+
    Row < Pending          , Leave         , Idle             , none           , none    >,
    Row < Pending          , Push          , Engaged          , RecordAnchor   , none    >,
    //  +------------------+-----------    +------------------+----------------+---------+
    Row < Engaged          , Release       , Pending          , none           , none    >,
    Row < Engaged          , EngagedEscape , EngagedRemotely  , none           , none    >,
    Row < Engaged          , Leave         , Idle             , none           , none    >,
    //  +------------------+-----------    +------------------+----------------+---------+
    Row < EngagedRemotely  , EngagedReturn , Engaged          , none           , none    >,
    Row < EngagedRemotely  , Release       , Idle             , none           , none    >,
    Row < EngagedRemotely  , Leave         , Idle             , none           , none    >
    //  +------------------+-----------    +------------------+----------------+---------+
    > {};

    // @formatter:on

private:
  Knob* _parent;

};

typedef state_machine<StateMachineFrontEnd> StateMachine;
}

Knob::Knob(Element::Dependencies elementDependencies)
  : Control(elementDependencies, "Knob"),
    _value(0.0),
    _valueAtAnchor(0.0)
{
  // Default track length is 4 inches
  _virtualTrackHeight = GetVPixels(4 * inches);

  // Create and store state machine
  auto stateMachine = new SmKnob::StateMachine(this);
  stateMachine->start();

  _stateMachine = stateMachine;
}

Knob::~Knob()
{
  // Delete state machine
  auto stateMachine = (SmKnob::StateMachine*) _stateMachine;
  delete stateMachine;
  _stateMachine = nullptr;
}

void Knob::SetVirtualTrackHeight(double trackHeight)
{
  _virtualTrackHeight = trackHeight;
}

double Knob::GetVirtualTrackHeight() const
{
  return _virtualTrackHeight;
}

void Knob::SetValue(double value)
{
  Knob::_value = value;
}

double Knob::GetValue() const
{
  return _value;
}

void Knob::SetValueChangedByInputCallback(
  const std::function<void(std::shared_ptr<Knob>)>& valueChangedByInputCallback)
{
  _valueChangedByInputCallback = valueChangedByInputCallback;
}

void Knob::OnValueChangedByInput()
{
  if (_valueChangedByInputCallback)
  {
    _valueChangedByInputCallback(std::static_pointer_cast<Knob>(shared_from_this()));
  }
}

void Knob::OnStateChangedByInput()
{
}

void Knob::NotifyInput(InputType inputType, InputAction inputAction, Point point)
{
  _inputPoint = point;

  auto stateMachine = (SmKnob::StateMachine*) _stateMachine;

  auto stateBefore = GetState();

  switch (inputAction)
  {
    case InputAction::EnterReleased:
      stateMachine->process_event(SmKnob::Enter());
      break;
    case InputAction::EnterPushed:
      stateMachine->process_event(SmKnob::Enter());
      break;
    case InputAction::Move:
      NotifyMove(point);
      break;
    case InputAction::Push:
      stateMachine->process_event(SmKnob::Push());
      break;
    case InputAction::Release:
      stateMachine->process_event(SmKnob::Release());
      break;
    case InputAction::Leave:
      stateMachine->process_event(SmKnob::Leave());
      break;
    case InputAction::EngagedEscape:
      stateMachine->process_event(SmKnob::EngagedEscape());
      NotifyMove(point);
      break;
    case InputAction::EngagedReturn:
      stateMachine->process_event(SmKnob::EngagedReturn());
      NotifyMove(point);
      break;
  }

  if (stateBefore != GetState())
  {
    OnStateChangedByInput();
  }

  if (InputAction::Move != inputAction)
  {
    // After any state changes except moving, we update the control (move updates are handled separately)
    UpdateAfterModify();
  }
}

void Knob::RecordAnchor()
{
  _anchorOffset = _inputPoint.Y;
  _valueAtAnchor = _value;
}

void Knob::NotifyMove(Point point)
{
  auto state = GetState();
  if (State::Engaged == state ||
      State::EngagedRemotely == state)
  {
    auto offsetPercent = (_anchorOffset - point.Y) / GetVirtualTrackHeight();
    if (0.0 != offsetPercent)
    {
      auto newValue = _valueAtAnchor + offsetPercent;
      newValue = std::max(0.0, newValue);
      newValue = std::min(1.0, newValue);
      SetValue(newValue);
      OnValueChangedByInput();
      UpdateAfterModify();
    }
  }
}

Knob::State Knob::GetState() const
{
  auto stateMachine = (SmKnob::StateMachine*) _stateMachine;
  return (State) stateMachine->current_state()[0];
}

}