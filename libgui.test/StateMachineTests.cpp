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

using namespace boost::msm::front;
using namespace boost::msm::back;
namespace mpl = boost::mpl;
using boost::msm::front::euml::func_state;
using emptyvector = boost::fusion::vector<>;
using boost::mpl::vector1;
using boost::mpl::vector2;
using boost::msm::TerminateFlag;

namespace SmTest // Avoid conflicts of same types used in different cpp files
{
// events
struct Down
{
};
struct Up
{
};
struct ExitToDisabled
{
  ExitToDisabled()
  {
  }

  // Support conversion from whatever event led to this one
  template<class Event>
  ExitToDisabled(Event const&)
  {
  }
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
      fflush(stdout);
    }

    // Enabled: first concurrent region
    struct CheckIsEnabled: public state<>
    {
      template<class Event, class Fsm>
      void on_entry(Event const&, Fsm&)
      {
        printf("EnterCheckIsEnabled\n");
        fflush(stdout);
      }

      template<class Event, class Fsm>
      void on_exit(Event const&, Fsm&)
      {
        printf("LeaveCheckIsEnabled\n");
        fflush(stdout);
      }
    };
    struct ToDisabled: public exit_pseudo_state<ExitToDisabled>
    {
      //typedef ::boost::mpl::vector< boost::msm::TerminateFlag>      internal_flag_list;
      //typedef ::boost::mpl::vector< boost::msm::TerminateFlag>      flag_list;

      template<class Event, class Fsm>
      void on_entry(Event const& evt, Fsm&)
      {
        printf("EnterToDisabled\n");
        fflush(stdout);
      }

      template<class Event, class Fsm>
      void on_exit(Event const&, Fsm&)
      {
        printf("LeaveToDisabled\n");
        fflush(stdout);
      }
    };

    // Enabled: second concurrent region
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

    // guards
    struct IsAtopDisabled
    {
      template<class EVT, class FSM, class SourceState, class TargetState>
      bool operator()(EVT const& evt, FSM& fsm, SourceState& ss, TargetState& ts)
      {
        return fsm._isAtopDisabled;
      }
    };

    // Replaces the default no-transition response.
    template<class FSM, class Event>
    void no_transition(Event const& e, FSM&, int state)
    {
      // Simply ignore any event that doesn't generate a transition
    }

    // Concurrent start states
    typedef mpl::vector<CheckIsEnabled, Pending> initial_state;

    // Transition table for state machine
    // @formatter:off
        struct transition_table : boost::mpl::vector<
        //    Start              Event           Next State         Action     Guard
        //  +------------------+-----------    +-----------------+----------- +-------------   +
        Row < CheckIsEnabled   , boost::any    , ToDisabled      , none       , IsAtopDisabled >,
        //  +------------------+-----------    +-----------------+----------- +-------------   +
        Row < Pending          , Down          , Engaged         , NotifyDown , none           >,
        //  +------------------+-----------    +-----------------+----------- +-------------   +
        Row < Engaged          , Up            , Pending         , NotifyUp   , none           >
        //  +------------------+-----------    +-----------------+----------- +-------------   +
        > {};
        // @formatter:on

    // States
    bool _isAtopDisabled = false;
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
    //    Start                   Event            Next State         Action                Guard
    //  +------------------     +-----------     +-----------------+-----------------------+-------------+
    Row < Enabled::exit_pt<
          Enabled_::ToDisabled> , ExitToDisabled , Disabled        , none                  , none        >
    //  +------------------     +-----------     +-----------------+-----------------------+-------------+
    > {};
    // @formatter:on

};

typedef state_machine<StateMachineFrontEnd> StateMachine;
}

TEST(StateMachineTests, Submachines)
{
  SmTest::StateMachine stateMachine;

  stateMachine.start();

  printf("\nSend Down Event...\n");
  fflush(stdout);
  stateMachine.process_event(SmTest::Down());

  printf("\nSend Up Event...\n");
  fflush(stdout);
  stateMachine.process_event(SmTest::Up());

  printf("\nSend Down Event...\n");
  fflush(stdout);
  stateMachine.process_event(SmTest::Down());

  printf("\n\n**Set Disabled**\n");
  fflush(stdout);
  auto& enabledState = stateMachine.get_state<SmTest::StateMachineFrontEnd::Enabled&>();
  enabledState._isAtopDisabled = true;

  printf("\nSend Up Event...\n");
  fflush(stdout);
  stateMachine.process_event(SmTest::Up());

}
