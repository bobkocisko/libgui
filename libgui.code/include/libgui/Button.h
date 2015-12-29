#pragma once 
#include "Control.h"
#include "Location.h"
#include "InputType.h"
#include "Point.h"

namespace libgui
{
    class Button: public Control
    {
    public:
        Button();
        virtual ~Button();

        // Input events
        void NotifyInput(InputType inputType, InputAction inputAction, Point point, bool& updateScreen) override;

        // This enum must match the order of the states defined in the state machine table
        enum VisibleState
        {
            Idle,
            Pending,
            Engaged,
            EngagedRemotely
        };

        VisibleState GetVisibleState();

        enum OutputEvent
        {
            Pushed,
            Released,
            Clicked
        };

        virtual void OnEvent(OutputEvent outputEvent);
        void         SetEventCallback(function<void(shared_ptr<Button>, OutputEvent)>);

    private:
        void * _stateMachine;

        function<void(shared_ptr<Button>, OutputEvent)>
                     _eventCallback;
    };

}
