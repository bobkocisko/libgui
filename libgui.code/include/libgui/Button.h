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

  virtual void OnEvent(OutputEvent outputEvent);
  void SetEventCallback(std::function<void(std::shared_ptr<Button>, OutputEvent)>);

  virtual std::string GetTypeName() override;
private:
  void* _stateMachine;

  std::function<void(std::shared_ptr<Button>, OutputEvent)>
      _eventCallback;
};

}
