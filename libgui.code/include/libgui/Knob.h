#pragma once

#include "Control.h"

#include "Location.h"
#include "InputAction.h"
#include "InputType.h"
#include "Point.h"

namespace libgui
{
class Knob: public Control
{
public:
  explicit Knob(Element::Dependencies elementDependencies);
  virtual ~Knob();

  /**
   * The knob control acts like a slider with a virtual track.  You can control
   * the resolution of the knob by increasing or decreasing the virtual track
   * height.  The default is 4 inches.
   */
  void SetVirtualTrackHeight(double trackHeight);
  double GetVirtualTrackHeight() const;

  /**
   * The knob control has a value which represents anything from 0.0 to 1.0.
   */
  void SetValue(double value);
  double GetValue() const;

  void SetValueChangedByInputCallback(
    const std::function<void(std::shared_ptr<Knob>)>& valueChangedByInputCallback);

  // Input events
  void NotifyInput(InputType inputType, InputAction inputAction, Point point) override;

  // States
  // This enum must match the order of the states defined in the state machine table
  enum State
  {
    Idle,
    Pending,
    Engaged,
    EngagedRemotely
  };
  State GetState() const;

  void RecordAnchor();

protected:
  virtual void OnValueChangedByInput();
  virtual void OnStateChangedByInput();

private:
  void* _stateMachine;
  double                _anchorOffset;
  Point                 _inputPoint;
  double                _virtualTrackHeight;
  double                _value;
  double                _valueAtAnchor;
  std::function<void(std::shared_ptr<Knob>)> _valueChangedByInputCallback;

  void NotifyMove(Point point);
};

}

