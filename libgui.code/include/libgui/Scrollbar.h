#pragma once
#include "Control.h"
#include "ScrollDelegate.h"
#include "Location.h"
#include "InputType.h"
#include "Point.h"

namespace libgui
{
class Scrollbar: public Element
{
public:
  explicit Scrollbar(Element::Dependencies elementDependencies, const std::shared_ptr<ScrollDelegate>& scrollDelegate);
  void PostConstruct();

  class Thumb;
  class Track;

  std::shared_ptr<Thumb> GetThumb() const;
  std::shared_ptr<Track> GetTrack() const;

  std::shared_ptr<ScrollDelegate> GetScrollDelegate() const;

  class Track: public Element
  {
  public:
    Track(Element::Dependencies elementDependencies);

    std::shared_ptr<Scrollbar> GetScrollbar() const;
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
    // As well as the funky id logic in GetState()
    enum State
    {
      Idle,
      Pending,
      Engaged,
      EngagedRemotely
    };
    State GetState() const;

    void Arrange() override;

    std::weak_ptr<Scrollbar> GetScrollbar() const;

    void RecordAnchor();

  private:
    void* _stateMachine;
    std::weak_ptr<Track>     _track;
    std::weak_ptr<Scrollbar> _scrollbar;
    double                   _anchorOffset;
    Point                    _pointer;

    void NotifyPointerMove(Point point);
  };
private:
  std::shared_ptr<Thumb>          _thumb;
  std::shared_ptr<Track>          _track;
  std::shared_ptr<ScrollDelegate> _scrollDelegate;
};
}

