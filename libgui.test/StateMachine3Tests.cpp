//
// Created by Bob Kocisko on 2/6/16.
//


#include <gtest/gtest.h>
#include <gmock/gmock.h>
using namespace std;

using ::testing::Return;


#include <boost/msm/front/states.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/euml/euml.hpp>
#include <type_traits>

using namespace boost::msm::front;
using namespace boost::msm::back;
namespace mpl = boost::mpl;
using boost::msm::front::euml::func_state;
using emptyvector = boost::fusion::vector<>;
using boost::mpl::vector1;
using boost::mpl::vector2;
using boost::msm::TerminateFlag;

namespace SmTest3 // Avoid conflicts of same types used in different cpp files
{
// events
struct Down
{
};
struct Up
{
};
struct TargetBecameDisabled
{
};

class StateMachineFrontEnd: public state_machine_def<StateMachineFrontEnd>
{

public:
    // states
    struct Disabled: public state<>
    {
        template<class Event, class Fsm>
        void on_entry(Event const&, Fsm&)
        {
            printf("EnterDisabled\n");
            fflush(stdout);
        }

        template<class Event, class Fsm>
        void on_exit(Event const&, Fsm&)
        {
            printf("LeaveDisabled\n");
            fflush(stdout);
        }
    };

    struct Fallthrough: public state<>
    {
        template<class Event, class Fsm>
        void on_entry(Event const& evt, Fsm&)
        {
            printf("EnterFallthrough\n");
            if (std::is_same<Event, TargetBecameDisabled>::value)
            {
                printf("FallthroughDuringDisabled\n");
            }
            if (std::is_same<Event, Down>::value)
            {
                printf("FallthroughDuringDown\n");
            }
            if (std::is_same<Event, boost::msm::front::none>::value)
            {
                printf("FallthroughDuring<none>\n");
            }
            fflush(stdout);
        }

        template<class Event, class Fsm>
        void on_exit(Event const&, Fsm&)
        {
            printf("LeaveFallthrough\n");
            fflush(stdout);
        }
    };

    struct Enabled_: public state_machine_def<Enabled_>
    {
        // (submachine)

        template<class Event, class Fsm>
        void on_entry(Event const&, Fsm&)
        {
            printf("EnterEnabled\n");
            fflush(stdout);
        }

        template<class Event, class Fsm>
        void on_exit(Event const&, Fsm&)
        {
            printf("LeaveEnabled\n");
            if (std::is_same<Event, TargetBecameDisabled>::value)
            {
                printf("LeavingDuringDisabled\n");
            }
            fflush(stdout);
        }

        struct Pending: public state<>
        {
            template<class Event, class Fsm>
            void on_entry(Event const&, Fsm&)
            {
                printf("EnterPending\n");
                fflush(stdout);
            }

            template<class Event, class Fsm>
            void on_exit(Event const&, Fsm&)
            {
                printf("LeavePending\n");
                fflush(stdout);
            }
        };
        struct Engaged: public state<>
        {
            template<class Event, class Fsm>
            void on_entry(Event const&, Fsm&)
            {
                printf("EnterEngaged\n");
                fflush(stdout);
            }

            template<class Event, class Fsm>
            void on_exit(Event const&, Fsm&)
            {
                printf("LeaveEngaged\n");
                fflush(stdout);
            }
        };

        // actions
        struct NotifyDown
        {
            template<class EVT, class FSM, class SourceState, class TargetState>
            void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
            {
                printf("Down\n");
                fflush(stdout);
            }
        };
        struct NotifyUp
        {
            template<class EVT, class FSM, class SourceState, class TargetState>
            void operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
            {
                printf("Up\n");
                fflush(stdout);
            }
        };

        // Replaces the default no-transition response.
        template<class FSM, class Event>
        void no_transition(Event const& e, FSM&, int state)
        {
            // Simply ignore any event that doesn't generate a transition
        }

        // Start state
        typedef mpl::vector<Pending> initial_state;

        // Transition table for state machine
        // @formatter:off
        struct transition_table : boost::mpl::vector<
        //    Start              Event           Next State         Action     Guard
        //  +------------------+-----------    +-----------------+----------- +-------------   +
        Row < Pending          , Down          , Engaged         , NotifyDown , none           >,
        //  +------------------+-----------    +-----------------+----------- +-------------   +
        Row < Engaged          , Up            , Pending         , NotifyUp   , none           >
        //  +------------------+-----------    +-----------------+----------- +-------------   +
        > {};
        // @formatter:on
    };
    typedef state_machine<Enabled_> Enabled;

    // Replaces the default no-transition response.
    template<class FSM, class Event>
    void no_transition(Event const& e, FSM&, int state)
    {
        // Simply ignore any event that doesn't generate a transition
    }

    // Set up the starting state of the state machine
    typedef Enabled initial_state;

    // Transition table for state machine
    // @formatter:off
    struct transition_table : boost::mpl::vector<
    //    Start                   Event                  Next State         Action                Guard
    //  +------------------     +-----------           +-----------------+-----------------------+-------------+
    Row < Enabled               , boost::any           , Disabled        , none                  , none        >,
    Row < Disabled              , none                 , Fallthrough     , none                  , none        >
    //  +------------------     +-----------           +-----------------+-----------------------+-------------+
    > {};
    // @formatter:on

};

typedef state_machine<StateMachineFrontEnd> StateMachine;
}

TEST(StateMachine3Tests, Submachines3)
{
    SmTest3::StateMachine stateMachine;

    stateMachine.start();

    printf("\nSend Down Event...\n");
    fflush(stdout);
    stateMachine.process_event(SmTest3::Down());

    printf("\nSend Up Event...\n");
    fflush(stdout);
    stateMachine.process_event(SmTest3::Up());

    printf("\nSend Down Event...\n");
    fflush(stdout);
    stateMachine.process_event(SmTest3::Down());

    printf("\nSend Down Event...\n");
    fflush(stdout);
    stateMachine.process_event(SmTest3::Down());

}
