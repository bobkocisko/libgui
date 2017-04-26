#include <libgui/InputAction.h>
#include <libgui/InputType.h>
#include <libgui/Point.h>
#include "libgui/Button.h"
#include "libgui/ElementManager.h"
#include "libgui/Location.h"

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


namespace SmButton
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
  StateMachineFrontEnd(Button* parent)
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
  struct NotifyPushed
  {
    template<class EVT, class FSM, class SourceState, class TargetState>
    void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
    {
      fsm._parent->OnEvent(Button::Pushed);
    }
  };
  struct NotifyReleased
  {
    template<class EVT, class FSM, class SourceState, class TargetState>
    void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
    {
      fsm._parent->OnEvent(Button::Released);
      fsm._parent->OnEvent(Button::Clicked);
    }
  };
  struct NotifyReleasedOutside
  {
    template<class EVT, class FSM, class SourceState, class TargetState>
    void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
    {
      fsm._parent->OnEvent(Button::Released);
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


    // NOTE: The order of the states listed in this table must match the order in the VisualState enum
    struct transition_table : boost::mpl::vector<
    //    Start              Event           Next State         Action                Guard
    //  +------------------+-----------    +-----------------+-----------------------+-------------+
    Row < Idle             , Enter         , Pending         , none                  , none        >,
    //  +------------------+-----------    +-----------------+-----------------------+-------------+
    Row < Pending          , Leave         , Idle            , none                  , none        >,
    Row < Pending          , Push          , Engaged         , NotifyPushed          , none        >,
    //  +------------------+-----------    +-----------------+-----------------------+-------------+
    Row < Engaged          , Release       , Pending         , NotifyReleased        , none        >,
    Row < Engaged          , EngagedEscape , EngagedRemotely , none                  , none        >,
    Row < Engaged          , Leave         , Idle            , none                  , none        >,
    //  +------------------+-----------    +-----------------+-----------------------+-------------+
    Row < EngagedRemotely  , EngagedReturn , Engaged         , none                  , none        >,
    Row < EngagedRemotely  , Release       , Idle            , NotifyReleasedOutside , none        >,
    Row < EngagedRemotely  , Leave         , Idle            , none                  , none        >
    //  +------------------+-----------    +-----------------+-----------------------+-------------+
    > {};
    // @formatter:on

private:
  Button* _parent;

};

typedef state_machine<StateMachineFrontEnd> StateMachine;
}

Button::Button(Element::Dependencies elementDependencies)
  : Button(elementDependencies, "Button")
{
}

Button::Button(Element::Dependencies elementDependencies, const std::string& typeName)
  : Control(elementDependencies, typeName)
{
  // Create and store state machine
  auto stateMachine = new SmButton::StateMachine(this);
  stateMachine->start();

  _stateMachine = stateMachine;
}

Button::~Button()
{
  // Delete state machine
  auto stateMachine = (SmButton::StateMachine*) _stateMachine;
  delete stateMachine;
  _stateMachine = nullptr;
}

void Button::NotifyInput(InputType inputType, InputAction inputAction, Point point)
{
  auto stateMachine = (SmButton::StateMachine*) _stateMachine;

  switch (inputAction)
  {

    case InputAction::EnterReleased:
      stateMachine->process_event(SmButton::Enter());
      break;
    case InputAction::EnterPushed:
      stateMachine->process_event(SmButton::Enter());
      break;
    case InputAction::Move:
      // Don't care
      break;
    case InputAction::Push:
      stateMachine->process_event(SmButton::Push());
      break;
    case InputAction::Release:
      stateMachine->process_event(SmButton::Release());
      break;
    case InputAction::Leave:
      stateMachine->process_event(SmButton::Leave());
      break;
    case InputAction::EngagedEscape:
      stateMachine->process_event(SmButton::EngagedEscape());
      break;
    case InputAction::EngagedReturn:
      stateMachine->process_event(SmButton::EngagedReturn());
      break;
  }

  if (InputAction::Move != inputAction)
  {
    // After any state changes except moving, we update the control
    UpdateAfterModify();
  }
}

void Button::OnEvent(OutputEvent outputEvent)
{
  // Default implementation: invoke the callback
  if (_eventCallback)
  {
    _eventCallback(std::dynamic_pointer_cast<Button>(shared_from_this()), outputEvent);
  }
}

void Button::SetEventCallback(std::function<void(std::shared_ptr<Button>, OutputEvent)> clickCallback)
{
  _eventCallback = clickCallback;
}

Button::VisibleState Button::GetVisibleState()
{
  auto stateMachine = (SmButton::StateMachine*) _stateMachine;
  return (VisibleState) stateMachine->current_state()[0];
}

std::string Button::GetTypeName()
{
  return "Button";
}
}
