#include <libgui/InputAction.h>
#include <libgui/InputType.h>
#include <libgui/Point.h>
#include "libgui/Slider.h"
#include "libgui/ElementManager.h"

#include <boost/msm/front/states.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/euml/euml.hpp>
#include <libgui/ScopeExit.h>

using namespace boost::msm::front;
using namespace boost::msm::back;
using boost::msm::front::euml::func_state;
using emptyvector = boost::fusion::vector<>;
using boost::mpl::vector1;
using boost::mpl::vector2;
using boost::msm::TerminateFlag;

namespace libgui
{

Slider::Slider(Element::Dependencies elementDependencies)
  : Slider(elementDependencies, "Slider")
{
}

Slider::Slider(Element::Dependencies elementDependencies, const std::string& typeName)
  : Element(elementDependencies, typeName)
{
}

void Slider::PostConstruct()
{
  _track = this->CreateChild<Track>();
  _thumb = _track->CreateChild<Thumb>();

  #ifdef DBG
  printf("Initializing Slider\n");
  fflush(stdout);
  #endif
}

double Slider::GetValue() const
{
  return _value;
}

void Slider::SetValue(double value)
{
  auto oldValue = _value;

  _value = value;

  if (_valueIsChangingByInput && (value != oldValue))
  {
    OnValueChangedByInput();
  }
}

double Slider::GetThumbHeight() const
{
  return _thumbHeight;
}

void Slider::SetThumbHeight(double thumbHeight)
{
  _thumbHeight = thumbHeight;
}

double Slider::GetFinenessMultiplier() const
{
  return _finenessMultiplier;
}

void Slider::SetFinenessMultiplier(double finenessMultiplier)
{
  _finenessMultiplier = finenessMultiplier;
}

void Slider::SetValueChangedByInputCallback(
  const std::function<void(std::shared_ptr<Slider>)>& valueChangedByInputCallback)
{
  _valueChangedByInputCallback = valueChangedByInputCallback;
}

std::shared_ptr<Slider::Thumb> Slider::GetThumb() const
{
  return _thumb;
}

std::shared_ptr<Slider::Track> Slider::GetTrack() const
{
  return _track;
}

namespace SmSlider
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

  StateMachineFrontEnd(Slider::Thumb* parent)
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
  Slider::Thumb* _parent;

};

typedef state_machine<StateMachineFrontEnd> StateMachine;
}

Slider::Track::Track(Element::Dependencies elementDependencies)
  : Element(elementDependencies, "Slider::Track")
{
}

std::shared_ptr<Slider> Slider::Track::GetSlider() const
{
  return std::dynamic_pointer_cast<Slider>(GetParent());
}

Slider::Thumb::Thumb(Element::Dependencies elementDependencies)
  : Control(elementDependencies, "Slider::Thumb"),
    _track(std::dynamic_pointer_cast<Track>(elementDependencies.parent)),
    _slider(_track.lock()->GetSlider())
{
  // Create and store state machine
  auto stateMachine = new SmSlider::StateMachine(this);
  stateMachine->start();

  _stateMachine = stateMachine;
}

Slider::Thumb::~Thumb()
{
  // Delete state machine
  auto stateMachine = (SmSlider::StateMachine*) _stateMachine;
  delete stateMachine;
  _stateMachine = nullptr;
}

void Slider::Thumb::Arrange()
{
  if (auto slider = _slider.lock())
  {
    auto p = GetParent();
    SetLeft(p->GetLeft());
    SetRight(p->GetRight());
    auto thumbHeight  = slider->GetThumbHeight();
    auto boundsTop    = p->GetTop();
    auto boundsHeight = p->GetHeight() - thumbHeight;
    auto top          = boundsTop + (slider->GetRawFromValue() * boundsHeight);
    SetTop(std::round(top));
    SetBottom(std::round(top + thumbHeight));
  }
}

void Slider::Thumb::NotifyInput(InputType inputType, InputAction inputAction, Point point)
{
  _inputPoint = point;

  auto stateMachine = (SmSlider::StateMachine*) _stateMachine;

  auto stateBefore = GetState();

  switch (inputAction)
  {
    case InputAction::EnterReleased:
      stateMachine->process_event(SmSlider::Enter());
      break;
    case InputAction::EnterPushed:
      stateMachine->process_event(SmSlider::Enter());
      break;
    case InputAction::Move:
      NotifyMove(point);
      break;
    case InputAction::Push:
      stateMachine->process_event(SmSlider::Push());
      break;
    case InputAction::Release:
      stateMachine->process_event(SmSlider::Release());
      break;
    case InputAction::Leave:
      stateMachine->process_event(SmSlider::Leave());
      break;
    case InputAction::EngagedEscape:
      stateMachine->process_event(SmSlider::EngagedEscape());
      NotifyMove(point);
      break;
    case InputAction::EngagedReturn:
      stateMachine->process_event(SmSlider::EngagedReturn());
      NotifyMove(point);
      break;
  }

  if (stateBefore != GetState())
  {
    if (auto slider = _slider.lock())
    {
      slider->OnThumbStateChangedByInput();
    }
  }

  if (InputAction::Move != inputAction)
  {
    // After any state changes except moving, we update the control (move updates are handled separately)
    UpdateAfterModify();
  }
}

void Slider::Thumb::RecordAnchor()
{
  auto slider = _slider.lock();
  if (!slider) return;

  _anchorOffset = _inputPoint.Y;
  _valueAtAnchor = slider->GetValue();
}

void Slider::Thumb::NotifyMove(Point point)
{
  auto state = GetState();
  if (State::Engaged == state ||
      State::EngagedRemotely == state)
  {
    auto slider = _slider.lock();
    auto track  = _track.lock();
    if (slider && track)
    {
      auto virtualTrackHeight = (track->GetHeight() - slider->GetThumbHeight());
      virtualTrackHeight *= slider->GetFinenessMultiplier();
      auto offsetPercent = (_anchorOffset - point.Y) / virtualTrackHeight;
      if (0.0 != offsetPercent)
      {
        auto newValue = _valueAtAnchor + offsetPercent;
        newValue = std::max(0.0, newValue);
        newValue = std::min(1.0, newValue);
        if (slider->GetValue() != newValue)
        {
          {
            slider->_valueIsChangingByInput = true;
            ScopeExit onScopeExit([slider]() {
              slider->_valueIsChangingByInput = false; });
            slider->SetValue(newValue);
          }
          UpdateAfterModify();
        }
      }
    }
  }
}

std::weak_ptr<Slider> Slider::Thumb::GetSlider() const
{
  return _slider;
}

void Slider::OnValueChangedByInput()
{
  if (_valueChangedByInputCallback)
  {
    _valueChangedByInputCallback(std::dynamic_pointer_cast<Slider>(shared_from_this()));
  }
}

void Slider::OnThumbStateChangedByInput()
{
}

double Slider::GetRawFromValue()
{
  return 1.0 - GetValue();
}

Slider::Thumb::State Slider::Thumb::GetState() const
{
  auto stateMachine = (SmSlider::StateMachine*) _stateMachine;
  return (State) stateMachine->current_state()[0];
}

const Inches Slider::GetThumbHeightInches() const
{
  return libgui::Inches(GetThumbHeight() / GetElementManager()->GetDpiY());
}

void Slider::SetThumbHeight(Inches thumbHeight)
{
  SetThumbHeight(double(thumbHeight) * GetElementManager()->GetDpiY());
}

void Slider::OnElementIsBeingRemoved()
{
  // Release anything held in a lambda capture
  _valueChangedByInputCallback = nullptr;
}
}