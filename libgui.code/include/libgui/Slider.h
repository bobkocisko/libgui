#pragma once
#include "Control.h"
#include "Location.h"
#include "InputAction.h"
#include "InputType.h"
#include "Point.h"

namespace libgui
{
	class Slider : public libgui::Element
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

		class Track : public Element
		{

		};

		class Thumb : public Control
		{
		public:
			explicit Thumb(weak_ptr<Slider> slider, weak_ptr<Track> track);

			// Input events
			void NotifyInput(InputAction inputAction, InputType inputType, Point point, bool& updateScreen) override;
			void NotifyMouseLeave() override;
			void NotifyMouseDown(Location location) override;
			void NotifyMouseUp(Location location) override;
			void NotifyMouseMove(Location location, bool& updateScreen) override;

			// Output states
			const bool& GetIsPressed() const;
			const bool& GetIsHot() const;

			void Arrange() override;

			const weak_ptr<Slider>& GetSlider() const;

		private:
			weak_ptr<Slider> m_slider;
			weak_ptr<Track> m_track;
			bool m_isOver;
			bool m_isPressed;
			bool m_isHot;
			double m_anchorOffset;
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
		virtual void SetValueFromRaw(double raw);

	private:
		shared_ptr<Thumb> m_thumb;
		shared_ptr<Track> m_track;
		double m_value = 0.0;

		double m_thumbHeight = 10.0; // Some default so that it is visible
		function<void(shared_ptr<Slider>)> m_valueChangedCallback;
	};
}

