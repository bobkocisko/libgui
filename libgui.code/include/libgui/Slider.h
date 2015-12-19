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
public:
    virtual void Init();

    const double& GetValue() const;
    void SetValue(double value);

    const double& GetThumbHeight() const;
    void SetThumbHeight(double thumbHeight);

    const function<void(shared_ptr<Slider>)>& GetValueChangedCallback() const;
    void SetValueChangedCallback(const function<void(shared_ptr<Slider>)>& valueChangedCallback);

    class Thumb;
    class Track;

    const shared_ptr<Thumb>& GetThumb() const;

    const shared_ptr<Track>& GetTrack() const;

    class Track: public Element
    {

    };

    class Thumb: public Control
    {
    public:
        explicit Thumb(weak_ptr<Slider> slider, weak_ptr<Track> track);
        virtual ~Thumb();

        // Input events
        void  NotifyInput(InputAction inputAction, InputType inputType, Point point, bool& updateScreen) override;

        // States
        // This enum must match the order of the states defined in the state machine table
        // As well as the funky id logic in GetState()
        enum State
        {
            Idle,
            Pending,
            Engaged
        };
        State GetState() const;

        void Arrange() override;

        const weak_ptr<Slider>& GetSlider() const;

        void RecordAnchor();
    private:
        void* _stateMachine;
        weak_ptr<Slider> _slider;
        weak_ptr<Track>  _track;
        double           _anchorOffset;
        Point            _pointer;

        void NotifyPointerMove(Point point, bool& updateScreen);
    };

protected:
    virtual void OnValueChanged();

    // Support transformations of the value from the raw 0.0-1.0 scale meaning a
    // linear top to bottom value.  The default transformation that makes 0.0 values
    // be at the bottom.
    virtual double GetRawFromValue();
    // Support transformations of the value from the raw 0.0-1.0 scale meaning a
    // linear top to bottom value.  The default transformation that makes 0.0 values
    // be at the bottom.
    virtual void   SetValueFromRaw(double raw);

private:
    shared_ptr<Thumb> _thumb;
    shared_ptr<Track> _track;
    double            _value = 0.0;

    double _thumbHeight = 10.0; // Some default so that it is visible
    function<void(shared_ptr<Slider>)>
           _valueChangedCallback;
};
}

