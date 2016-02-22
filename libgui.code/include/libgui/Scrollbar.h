#pragma once
#include "Control.h"
#include "ScrollDelegate.h"
#include "Location.h"
#include "InputType.h"
#include "Point.h"

namespace libgui
{
class Scrollbar: public libgui::Element
{
public:
    explicit Scrollbar(const shared_ptr<ScrollDelegate>& scrollDelegate);

    bool InitializeThis() override;

    class Thumb;
    class Track;

    const shared_ptr<Thumb>& GetThumb() const;
    const shared_ptr<Track>& GetTrack() const;

    const shared_ptr<ScrollDelegate>& GetScrollDelegate() const;
    void SetScrollDelegate(const shared_ptr<ScrollDelegate>& scrollDelegate);

    virtual std::string GetTypeName() override;

    class Track: public Element
    {
    public:
        virtual std::string GetTypeName() override;
    };

    class Thumb: public Control
    {
    public:
        explicit Thumb(weak_ptr<Scrollbar> scrollbar, weak_ptr<Track> track);
        virtual ~Thumb();

        // Input events
        void  NotifyInput(InputType inputType, InputAction inputAction, Point point) override;

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

        const weak_ptr<Scrollbar>& GetScrollbar() const;

        void RecordAnchor();

        virtual std::string GetTypeName() override;

    private:
        void* _stateMachine;
        weak_ptr<Scrollbar> _scrollbar;
        weak_ptr<Track>     _track;
        double              _anchorOffset;
        Point               _pointer;

        void NotifyPointerMove(Point point);
    };
private:
    shared_ptr<Thumb>          _thumb;
    shared_ptr<Track>          _track;
    shared_ptr<ScrollDelegate> _scrollDelegate;
};
}

