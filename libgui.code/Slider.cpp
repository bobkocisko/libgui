#include <libgui/InputAction.h>
#include <libgui/InputType.h>
#include <libgui/Point.h>
#include "libgui/Common.h"
#include "libgui/Slider.h"
#include "libgui/ElementManager.h"

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
void Slider::Initialize()
{
    _track = make_shared<Track>();
    this->AddChild(_track);

    _thumb = make_shared<Thumb>(
        dynamic_pointer_cast<Slider>(shared_from_this()),
        _track);
    _track->AddChild(_thumb);
}

const double& Slider::GetValue() const
{
    return _value;
}

void Slider::SetValue(double value)
{
    auto oldValue = _value;

    _value = value;

    if (value != oldValue)
    {
        OnValueChanged();
    }
}

const double& Slider::GetThumbHeight() const
{
    return _thumbHeight;
}

void Slider::SetThumbHeight(double thumbHeight)
{
    _thumbHeight = thumbHeight;
}

const function<void(shared_ptr<Slider>)>& Slider::GetValueChangedCallback() const
{
    return _valueChangedCallback;
}

void Slider::SetValueChangedCallback(const function<void(shared_ptr<Slider>)>& valueChangedCallback)
{
    _valueChangedCallback = valueChangedCallback;
}

const shared_ptr<Slider::Thumb>& Slider::GetThumb() const
{
    return _thumb;
}

const shared_ptr<Slider::Track>& Slider::GetTrack() const
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
    //  +------------------+-----------    +------------------+----------------+---------+
    Row < EngagedRemotely  , EngagedReturn , Engaged          , none           , none    >,
    Row < EngagedRemotely  , Release       , Idle             , none           , none    >
    //  +------------------+-----------    +------------------+----------------+---------+
    > {};

    // @formatter:on

private:
    Slider::Thumb* _parent;

};

typedef state_machine<StateMachineFrontEnd> StateMachine;
}

Slider::Thumb::Thumb(weak_ptr<Slider> slider, weak_ptr<Track> track)
    : _slider(slider),
      _track(track)
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
        SetTop(round(top));
        SetBottom(round(top + thumbHeight));
    }
}

void Slider::Thumb::NotifyInput(InputType inputType, InputAction inputAction, Point point, bool& updateScreen)
{
    // Apply the default screen update logic
    Control::NotifyInput(inputType, inputAction, point, updateScreen);

    _inputPoint = point;

    auto stateMachine = (SmSlider::StateMachine*) _stateMachine;

    switch (inputAction)
    {
        case InputAction::EnterReleased:
            stateMachine->process_event(SmSlider::Enter());
            break;
        case InputAction::EnterPushed:
            stateMachine->process_event(SmSlider::Enter());
            break;
        case InputAction::Move:
            bool moveUpdateScreen;
            NotifyMove(point, moveUpdateScreen);
            if (moveUpdateScreen)
            {
                updateScreen = true;
            }
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
            break;
        case InputAction::EngagedReturn:
            stateMachine->process_event(SmSlider::EngagedReturn());
            break;
    }
}

void Slider::Thumb::RecordAnchor()
{
    _anchorOffset = _inputPoint.Y - GetTop();
}

void Slider::Thumb::NotifyMove(Point point, bool& updateScreen)
{
    updateScreen = false;
    auto state = GetState();
    if (State::Engaged == state ||
        State::EngagedRemotely == state)
    {
        auto slider = _slider.lock();
        auto track  = _track.lock();
        if (slider && track)
        {
            auto offsetPercent = ((point.Y - _anchorOffset) - track->GetTop()) /
                                 (track->GetHeight() - slider->GetThumbHeight());
            offsetPercent      = max(0.0, offsetPercent);
            offsetPercent      = min(1.0, offsetPercent);
            if (slider->GetRawFromValue() != offsetPercent)
            {
                slider->SetValueFromRaw(offsetPercent);
                updateScreen = true;
            }
        }
    }
}

const weak_ptr<Slider>& Slider::Thumb::GetSlider() const
{
    return _slider;
}

void Slider::OnValueChanged()
{
    if (_valueChangedCallback)
    {
        _valueChangedCallback(dynamic_pointer_cast<Slider>(shared_from_this()));
    }
}

double Slider::GetRawFromValue()
{
    return 1.0 - GetValue();
}

void Slider::SetValueFromRaw(double raw)
{
    SetValue(1.0 - raw);
}

Slider::Thumb::State Slider::Thumb::GetState() const
{
    auto stateMachine = (SmSlider::StateMachine*) _stateMachine;
    return (State) stateMachine->current_state()[0];
}

const inches Slider::GetThumbHeightInches() const
{
    return libgui::inches(GetThumbHeight() / GetElementManager()->GetDpiY());
}

void Slider::SetThumbHeight(inches thumbHeight)
{
    SetThumbHeight(double(thumbHeight) * GetElementManager()->GetDpiY());
}
}