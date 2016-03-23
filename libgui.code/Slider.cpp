#include <libgui/InputAction.h>
#include <libgui/InputType.h>
#include <libgui/Point.h>
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

bool Slider::InitializeThis()
{
    if (!Element::InitializeThis())
    {
        return false;
    }

    _track = std::make_shared<Track>();
    this->AddChild(_track);

    _thumb = std::make_shared<Thumb>(
        std::dynamic_pointer_cast<Slider>(shared_from_this()),
        _track);
    _track->AddChild(_thumb);

    #ifdef DBG
    printf("Initializing Slider\n");
    fflush(stdout);
    #endif

    return true;
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

const std::function<void(std::shared_ptr<Slider>)>& Slider::GetValueChangedCallback() const
{
    return _valueChangedCallback;
}

void Slider::SetValueChangedCallback(const std::function<void(std::shared_ptr<Slider>)>& valueChangedCallback)
{
    _valueChangedCallback = valueChangedCallback;
}

const std::shared_ptr<Slider::Thumb>& Slider::GetThumb() const
{
    return _thumb;
}

const std::shared_ptr<Slider::Track>& Slider::GetTrack() const
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

Slider::Thumb::Thumb(std::weak_ptr<Slider> slider, std::weak_ptr<Track> track)
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

void Slider::Thumb::NotifyInput(InputType inputType, InputAction inputAction, Point point)
{
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

    if (InputAction::Move != inputAction)
    {
        // After any state changes except moving, we update the control (move updates are handled separately)
        Update(UpdateType::Modifying);
    }
}

void Slider::Thumb::RecordAnchor()
{
    _anchorOffset = _inputPoint.Y - GetTop();
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
            auto offsetPercent = ((point.Y - _anchorOffset) - track->GetTop()) /
                                 (track->GetHeight() - slider->GetThumbHeight());
            offsetPercent = std::max(0.0, offsetPercent);
            offsetPercent = std::min(1.0, offsetPercent);
            if (slider->GetRawFromValue() != offsetPercent)
            {
                slider->SetValueFromRaw(offsetPercent);
                Update(UpdateType::Modifying);
            }
        }
    }
}

const std::weak_ptr<Slider>& Slider::Thumb::GetSlider() const
{
    return _slider;
}

void Slider::OnValueChanged()
{
    if (_valueChangedCallback)
    {
        _valueChangedCallback(std::dynamic_pointer_cast<Slider>(shared_from_this()));
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

const Inches Slider::GetThumbHeightInches() const
{
    return libgui::Inches(GetThumbHeight() / GetElementManager()->GetDpiY());
}

void Slider::SetThumbHeight(Inches thumbHeight)
{
    SetThumbHeight(double(thumbHeight) * GetElementManager()->GetDpiY());
}

std::string Slider::GetTypeName()
{
    return "Slider";
}

std::string Slider::Track::GetTypeName()
{
    return "Track";
}

std::string Slider::Thumb::GetTypeName()
{
    return "Thumb";
}
}