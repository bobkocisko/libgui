#include <libgui/InputAction.h>
#include <libgui/InputType.h>
#include <libgui/Point.h>
#include "libgui/Common.h"
#include "libgui/Scrollbar.h"
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
Scrollbar::Scrollbar(const shared_ptr<ScrollDelegate>& scrollDelegate)
    : _scrollDelegate(scrollDelegate)
{
}

void Scrollbar::Init()
{
    _track = make_shared<Track>();
    this->AddChild(_track);

    _thumb = make_shared<Thumb>(
        dynamic_pointer_cast<Scrollbar>(shared_from_this()),
        _track);
    _track->AddChild(_thumb);
}

const shared_ptr<Scrollbar::Thumb>& Scrollbar::GetThumb() const
{
    return _thumb;
}

const shared_ptr<Scrollbar::Track>& Scrollbar::GetTrack() const
{
    return _track;
}

const shared_ptr<ScrollDelegate>& Scrollbar::GetScrollDelegate() const
{
    return _scrollDelegate;
}

void Scrollbar::SetScrollDelegate(const shared_ptr<ScrollDelegate>& scrollDelegate)
{
    _scrollDelegate = scrollDelegate;
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
    //    Start              Event       Next State         Action           Guard
    //  +------------------+-----------+------------------+----------------+---------+
    Row < Idle             , Enter     , Pending          , none           , none    >,
    //  +------------------+-----------+------------------+----------------+---------+
    Row < Pending          , Leave     , Idle             , none           , none    >,
    Row < Pending          , Push      , Engaged          , RecordAnchor   , none    >,
    //  +------------------+-----------+------------------+----------------+---------+
    Row < Engaged          , Release   , Pending          , none           , none    >,
    Row < Engaged          , Leave     , EngagedRemotely  , none           , none    >,
    //  +------------------+-----------+------------------+----------------+---------+
    Row < EngagedRemotely  , Enter     , Engaged          , none           , none    >,
    Row < EngagedRemotely  , Release   , Idle             , none           , none    >
    //  +------------------+-----------+------------------+----------------+---------+
    > {};

    // @formatter:on

private:
    Scrollbar::Thumb* _parent;

};

typedef state_machine<StateMachineFrontEnd> StateMachine;
}

Scrollbar::Thumb::Thumb(weak_ptr<Scrollbar> scrollbar, weak_ptr<Track> track)
    : _scrollbar(scrollbar),
      _track(track)
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
        SetTop(round(top));
        SetBottom(round(top + height));
    }
}

void Scrollbar::Thumb::NotifyInput(InputType inputType, InputAction inputAction, Point point, bool& updateScreen)
{
    if (InputType::Touch == inputType)
    {
        // Biased judgement: Scrollbars shouldn't even be visible in touch mode, so ignore touch inputs
        updateScreen = false;
        return;
    }

    // Apply the default screen update logic
    Control::NotifyInput(inputType, inputAction, point, updateScreen);

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
            bool moveUpdateScreen;
            NotifyPointerMove(point, moveUpdateScreen);
            if (moveUpdateScreen)
            {
                updateScreen = true;
            }
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
    }

}

void Scrollbar::Thumb::RecordAnchor()
{
    _anchorOffset = _pointer.Y - GetTop();
}

void Scrollbar::Thumb::NotifyPointerMove(Point point, bool& updateScreen)
{
    updateScreen = false;
    _pointer     = point;
    if (State::Engaged == GetState())
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
                updateScreen = true;
            }
        }
    }
}

const weak_ptr<Scrollbar>& Scrollbar::Thumb::GetScrollbar() const
{
    return _scrollbar;
}

Scrollbar::Thumb::State Scrollbar::Thumb::GetState() const
{
    auto stateMachine = (SmScrollbar::StateMachine*) _stateMachine;
    auto currentState = stateMachine->current_state()[0];
    if (3 == currentState) // Treat EngagedRemotely and Engaged as the same state
    {
        currentState = 2;
    }
    return (State) currentState;
}
}