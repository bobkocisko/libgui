#include <libgui/InputAction.h>
#include <libgui/InputType.h>
#include <libgui/Point.h>
#include "libgui/Common.h"
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


    namespace sm
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
            StateMachineFrontEnd(Button* parent)
                : _parent(parent)
            {
            }

            // states
            struct Cold_Up: public state<>
            {
            };
            struct Hot_Up: public state<>
            {
            };
            struct Hot_Down: public state<>
            {
            };
            struct Cold_Claimed: public state<>
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
            typedef Cold_Up initial_state;


            // Transition table for state machine
            // @formatter:off


            // NOTE: The order of the states listed in this table must match the order in the VisualState enum
            struct transition_table : boost::mpl::vector<
            //    Start              Event       Next State         Action                 Guard
            //  +------------------+-----------+------------------+------------------------+-------------+
            Row < Cold_Up          , Enter     , Hot_Up           , none                   , none        >,
            //  +------------------+-----------+------------------+------------------------+-------------+
            Row < Hot_Up           , Leave     , Cold_Up          , none                   , none        >,
            Row < Hot_Up           , Push      , Hot_Down         , NotifyPushed           , none        >,
            //  +------------------+-----------+------------------+------------------------+-------------+
            Row < Hot_Down         , Release   , Hot_Up           , NotifyReleased         , none        >,
            Row < Hot_Down         , Leave     , Cold_Claimed     , none                   , none        >,
            //  +------------------+-----------+------------------+------------------------+-------------+
            Row < Cold_Claimed     , Enter     , Hot_Down         , none                   , none        >,
            Row < Cold_Claimed     , Release   , Cold_Up          , NotifyReleasedOutside  , none        >,
            //  +------------------+-----------+------------------+------------------------+-------------+
            > {};
            // @formatter:on

        private:
            Button* _parent;

        };

        typedef state_machine<StateMachineFrontEnd> StateMachine;
    }

    Button::Button()
    {
        // Create and store state machine
        auto stateMachine = new sm::StateMachine(this);
        stateMachine->start();

        _stateMachine = stateMachine;
    }

    virtual Button::~Button()
    {
        // Delete state machine
        auto stateMachine = (sm::StateMachine*) _stateMachine;
        delete stateMachine;
        _stateMachine = nullptr;
    }

    void Button::NotifyInput(InputAction inputAction, InputType inputType, Point point, bool& updateScreen)
    {
        // Apply the default screen update logic
        Control::NotifyInput(inputAction, inputType, point, updateScreen);

        auto stateMachine = (sm::StateMachine*) _stateMachine;

        switch (inputAction)
        {

            case InputAction::EnterReleased:
                stateMachine->process_event(sm::Enter());
                break;
            case InputAction::EnterPushed:
                stateMachine->process_event(sm::Enter());
                break;
            case InputAction::Move:
                // Don't care
                break;
            case InputAction::Push:
                stateMachine->process_event(sm::Push());
                break;
            case InputAction::Release:
                stateMachine->process_event(sm::Release());
                break;
            case InputAction::Leave:
                stateMachine->process_event(sm::Leave());
                break;
        }

    }

    void Button::OnEvent(OutputEvent outputEvent)
    {
        // Default implementation: invoke the callback
        if (_eventCallback)
        {
            _eventCallback(dynamic_pointer_cast<Button>(shared_from_this()), outputEvent);
        }
    }

    void Button::SetEventCallback(function<void(shared_ptr<Button>, OutputEvent)> clickCallback)
    {
        _eventCallback = clickCallback;
    }

    Button::VisibleState Button::GetVisibleState()
    {
        auto stateMachine = (sm::StateMachine*) _stateMachine;
        return (VisibleState)stateMachine->current_state()[0];
    }
}
