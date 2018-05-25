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
  Button(Element::Dependencies elementDependencies);
  Button(Element::Dependencies elementDependencies, const std::string& typeName);

  virtual ~Button();

  // Input events
  void NotifyInput(InputType inputType, InputAction inputAction, Point point) override;

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

  // A child class can override this to receive information about events
  // occurring on this button.  (NOTE: Invoking this method is reserved
  // to internal use only.  It is public because of an implementation detail).
  virtual void OnEvent(OutputEvent outputEvent);

  // Specify a callback which will be called to receive information about events
  // occurring on this button.
  void SetEventCallback(std::function<void(std::shared_ptr<Button>, OutputEvent)>);

  virtual std::string GetTypeName() override;

protected:

  // Cleanup
  void OnElementIsBeingRemoved() override;

private:
  void* _stateMachine;

  std::function<void(std::shared_ptr<Button>, OutputEvent)>
      _eventCallback;
};

}
