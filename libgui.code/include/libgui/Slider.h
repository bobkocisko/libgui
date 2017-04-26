#pragma once
#include "Control.h"
#include "Location.h"
#include "InputAction.h"
#include "InputType.h"
#include "Point.h"

namespace libgui
{
class Slider: public libgui::Element
{
  friend class Thumb;
public:
  explicit Slider(Element::Dependencies elementDependencies);
  void PostConstruct();

  double GetValue() const;
  void SetValue(double value);

  double GetThumbHeight() const;
  void SetThumbHeight(double thumbHeight);

  const Inches GetThumbHeightInches() const;
  void SetThumbHeight(Inches thumbHeight);

  void SetValueChangedByInputCallback(const std::function<void(std::shared_ptr<Slider>)>& valueChangedByInputCallback);

  class Thumb;
  class Track;

  std::shared_ptr<Thumb> GetThumb() const;

  std::shared_ptr<Track> GetTrack() const;

  class Track: public Element
  {
  public:
    explicit Track(Element::Dependencies elementDependencies);
    std::shared_ptr<Slider> GetSlider() const;
  };

  class Thumb: public Control
  {
  public:
    explicit Thumb(Element::Dependencies elementDependencies);
    virtual ~Thumb();

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

    void Arrange() override;

    std::weak_ptr<Slider> GetSlider() const;

    void RecordAnchor();

  private:
    void* _stateMachine;
    std::weak_ptr<Track>  _track;
    std::weak_ptr<Slider> _slider;
    double                _anchorOffset;
    Point                 _inputPoint;

    void NotifyMove(Point point);
  };

protected:
  virtual void OnValueChangedByInput();

  // Support transformations of the value from the raw 0.0-1.0 scale meaning a
  // linear top to bottom value.  The default transformation that makes 0.0 values
  // be at the bottom.
  virtual double GetRawFromValue();
  // Support transformations of the value from the raw 0.0-1.0 scale meaning a
  // linear top to bottom value.  The default transformation that makes 0.0 values
  // be at the bottom.
  virtual void SetValueFromRaw(double raw);

private:
  std::shared_ptr<Thumb> _thumb;
  std::shared_ptr<Track> _track;
  double                 _value                  = 0.0;
  bool                   _valueIsChangingByInput = false;
  double                 _thumbHeight            = 10.0; // Some default so that it is visible

  std::function<void(std::shared_ptr<Slider>)>
                         _valueChangedByInputCallback;
};
}

