#include <libgui/InputAction.h>
#include <libgui/InputType.h>
#include <libgui/Point.h>
#include "libgui/Scrollbar.h"
#include "libgui/ElementManager.h"
#include "libgui/Location.h"

#include <boost/msm/front/states.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/euml/euml.hpp>
#include <cmath>

using namespace boost::msm::front;
using namespace boost::msm::back;
using boost::msm::front::euml::func_state;
using emptyvector = boost::fusion::vector<>;
using boost::mpl::vector1;
using boost::mpl::vector2;
using boost::msm::TerminateFlag;

namespace libgui
{
Scrollbar::Scrollbar(Element::Dependencies elementDependencies, const std::shared_ptr<ScrollDelegate>& scrollDelegate)
  : Element(elementDependencies, "Scrollbar"),
    _scrollDelegate(scrollDelegate)
{
  assert(scrollDelegate);
}

void Scrollbar::PostConstruct()
{
  _track = this->CreateChild<Track>();
  _thumb = _track->CreateChild<Thumb>();

  std::weak_ptr<Thumb> weakThumb(std::static_pointer_cast<Thumb>(_thumb));
  _scrollDelegate->WhenThumbSizePercentChanges([this, weakThumb] {
    auto thumb = weakThumb.lock();
    if (!thumb) return;
    thumb->UpdateAfterModify();
  });
}

std::shared_ptr<Scrollbar::Thumb> Scrollbar::GetThumb() const
{
  return _thumb;
}

std::shared_ptr<Scrollbar::Track> Scrollbar::GetTrack() const
{
  return _track;
}

std::shared_ptr<ScrollDelegate> Scrollbar::GetScrollDelegate() const
{
  return _scrollDelegate;
}

namespace SmScrollbar
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
  StateMachineFrontEnd(Scrollbar::Thumb* parent)
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
    // as well as the funky logic in GetState().
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
  Scrollbar::Thumb* _parent;

};

typedef state_machine<StateMachineFrontEnd> StateMachine;
}

Scrollbar::Track::Track(Element::Dependencies elementDependencies)
  : Element(elementDependencies, "Scrollbar::Track")
{
}

std::shared_ptr<Scrollbar> Scrollbar::Track::GetScrollbar() const
{
  return std::dynamic_pointer_cast<Scrollbar>(GetParent());
}

Scrollbar::Thumb::Thumb(Element::Dependencies elementDependencies)
  : Control(elementDependencies, "Scrollbar::Thumb"),
    _track(std::dynamic_pointer_cast<Track>(elementDependencies.parent)),
    _scrollbar(_track.lock()->GetScrollbar())

{
  // Create and store state machine
  auto stateMachine = new SmScrollbar::StateMachine(this);
  stateMachine->start();

  _stateMachine = stateMachine;
}

Scrollbar::Thumb::~Thumb()
{
  // Delete state machine
  auto stateMachine = (SmScrollbar::StateMachine*) _stateMachine;
  delete stateMachine;
  _stateMachine = nullptr;
}

void Scrollbar::Thumb::Arrange()
{
  if (auto sb = _scrollbar.lock())
  {
    auto scrollDelegate = sb->GetScrollDelegate();

    auto p = GetParent();
    SetLeft(p->GetLeft());
    SetRight(p->GetRight());
    auto trackHeight = p->GetHeight();
    auto height      = scrollDelegate->GetThumbSizePercent() * trackHeight;
    auto top         = p->GetTop() + (scrollDelegate->GetCurrentOffsetPercent() * trackHeight);
    SetTop(std::round(top));
    SetBottom(std::round(top + height));
  }
}

void Scrollbar::Thumb::NotifyInput(InputType inputType, InputAction inputAction, Point point)
{
  _inputPoint = point;

  auto stateMachine = (SmScrollbar::StateMachine*) _stateMachine;

  switch (inputAction)
  {

    case InputAction::EnterReleased:
      stateMachine->process_event(SmScrollbar::Enter());
      break;
    case InputAction::EnterPushed:
      stateMachine->process_event(SmScrollbar::Enter());
      break;
    case InputAction::Move:
      NotifyMove(point);
      break;
    case InputAction::Push:
      stateMachine->process_event(SmScrollbar::Push());
      break;
    case InputAction::Release:
      stateMachine->process_event(SmScrollbar::Release());
      break;
    case InputAction::Leave:
      stateMachine->process_event(SmScrollbar::Leave());
      break;
    case InputAction::EngagedEscape:
      stateMachine->process_event(SmScrollbar::EngagedEscape());
      NotifyMove(point);
      break;
    case InputAction::EngagedReturn:
      stateMachine->process_event(SmScrollbar::EngagedReturn());
      NotifyMove(point);
      break;
  }

  if (InputAction::Move != inputAction)
  {
    // After any state changes except moving, we update the control (move updates are handled separately)
    UpdateAfterModify();
  }
}

void Scrollbar::Thumb::RecordAnchor()
{
  _anchorOffset = _inputPoint.Y - GetTop();
}

void Scrollbar::Thumb::NotifyMove(Point point)
{
  auto state = GetState();
  if (State::Engaged == state ||
      State::EngagedRemotely == state)
  {
    auto sb    = _scrollbar.lock();
    auto track = _track.lock();
    if (sb && track)
    {
      auto scrollDelegate = sb->GetScrollDelegate();
      auto offsetPercent  = ((point.Y - _anchorOffset) - track->GetTop()) / track->GetHeight();
      scrollDelegate->LimitToBounds(offsetPercent);
      if (scrollDelegate->GetCurrentOffsetPercent() != offsetPercent)
      {
        scrollDelegate->MoveToOffsetPercent(offsetPercent);
        if (auto e = std::dynamic_pointer_cast<Element>(scrollDelegate))
        {
          e->UpdateAfterModify();
        }
        UpdateAfterModify();
      }
    }
  }
}

std::weak_ptr<Scrollbar> Scrollbar::Thumb::GetScrollbar() const
{
  return _scrollbar;
}

Scrollbar::Thumb::State Scrollbar::Thumb::GetState() const
{
  auto stateMachine = (SmScrollbar::StateMachine*) _stateMachine;
  return (State) stateMachine->current_state()[0];
}
}