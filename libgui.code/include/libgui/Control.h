#pragma once
#include "Element.h"
#include "Location.h"
#include "InputAction.h"
#include "InputType.h"
#include "Point.h"


namespace libgui
{
class Input;

class Control: public Element
{
public:
  Control(Element::Dependencies elementDependencies);
  Control(Element::Dependencies elementDependencies, const std::string& typeName);

  virtual ~Control();

  // Called before NotifyInput and can optionally prevent the sending of NotifyInput event
  virtual void PreNotifyInput(InputAction inputAction,
                              InputType inputType,
                              Point point,
                              bool& notify);

  // Called when an input action occurs.  See ElementManager for details of when this is called.
  virtual void NotifyInput(InputType inputType,
                           InputAction inputAction,
                           Point point);

  // Input Management
  bool HasActiveInput() const;
  void SetHasActiveInput(bool hasActiveInput);

private:
  bool _hasActiveInput = false;

};
}
