#include "Precompiled.h"
#include "Slider.h"
#include "ElementManager.h"

namespace libgui
{
	void Slider::Init()
	{
		m_track = make_shared<Track>();
		this->AddChild(m_track);

		m_thumb = make_shared<Thumb>(
			dynamic_pointer_cast<Slider>(shared_from_this()),
			m_track);
		m_track->AddChild(m_thumb);
	}

	const double& Slider::GetValue() const
	{
		return m_value;
	}

	void Slider::SetValue(double value)
	{
		auto oldValue = m_value;

		m_value = value;

		if (value != oldValue)
		{
			OnValueChanged();
		}
	}

	const double& Slider::GetThumbHeight() const
	{
		return m_thumbHeight;
	}

	void Slider::SetThumbHeight(double thumbHeight)
	{
		m_thumbHeight = thumbHeight;
	}

	const function<void(shared_ptr<Slider>)>& Slider::GetValueChangedCallback() const
	{
		return m_valueChangedCallback;
	}

	void Slider::SetValueChangedCallback(const function<void(shared_ptr<Slider>)>& valueChangedCallback)
	{
		m_valueChangedCallback = valueChangedCallback;
	}

	const shared_ptr<Slider::Thumb>& Slider::GetThumb() const
	{
		return m_thumb;
	}

	const shared_ptr<Slider::Track>& Slider::GetTrack() const
	{
		return m_track;
	}

	Slider::Thumb::Thumb(weak_ptr<Slider> slider, weak_ptr<Track> track)
		: m_slider(slider),
		m_track(track)
	{
	}

	void Slider::Thumb::Arrange()
	{
		if (auto slider = m_slider.lock())
		{
			auto p = GetParent();
			SetLeft(p->GetLeft()); SetRight(p->GetRight());
			auto thumbHeight = slider->GetThumbHeight();
			auto boundsTop = p->GetTop();
			auto boundsHeight = p->GetHeight() - thumbHeight;
			auto top = boundsTop + (slider->GetRawFromValue() * boundsHeight);
			SetTop(round(top)); SetBottom(round(top + thumbHeight));
		}
	}

	void Slider::Thumb::NotifyMouseEnter()
	{
		m_isOver = true;

		if (IsCapturing())
		{
			m_isPressed = true;
		}
		else
		{
			m_isHot = true;
		}
	}

	void Slider::Thumb::NotifyMouseLeave()
	{
		m_isOver = false;

		if (IsCapturing())
		{
			m_isPressed = false;
		}
		else
		{
			m_isHot = false;
		}
	}

	void Slider::Thumb::NotifyMouseDown(Location location)
	{
		m_isHot = true;
		m_isPressed = true;

		m_anchorOffset = location.y - GetTop();

		GetElementManager()->RequestCapture(dynamic_pointer_cast<Control>(shared_from_this()));
	}

	void Slider::Thumb::NotifyMouseUp(Location location)
	{
		m_isPressed = false;
		if (IsCapturing())
		{
			if (!m_isOver)
			{
				m_isHot = false;
			}

			GetElementManager()->ReleaseCapture();
		}
	}

	void Slider::Thumb::NotifyMouseMove(Location location, bool& updateScreen)
	{
		if (IsCapturing())
		{
			auto slider = m_slider.lock();
			auto track = m_track.lock();
			if (slider && track)
			{
				auto offsetPercent = ((location.y - m_anchorOffset) - track->GetTop()) / 
					(track->GetHeight() - slider->GetThumbHeight());
				offsetPercent = max(0.0, offsetPercent);
				offsetPercent = min(1.0, offsetPercent);
				if (slider->GetRawFromValue() != offsetPercent)
				{
					slider->SetValueFromRaw(offsetPercent);
					updateScreen = true;
					return;
				}
			}
		}
		updateScreen = false;
		return;
	}

	const bool& Slider::Thumb::GetIsPressed() const
	{
		return m_isPressed;
	}

	const bool& Slider::Thumb::GetIsHot() const
	{
		return m_isHot;
	}

	const weak_ptr<Slider>& Slider::Thumb::GetSlider() const
	{
		return m_slider;
	}

	void Slider::OnValueChanged()
	{
		if (m_valueChangedCallback)
		{
			m_valueChangedCallback(dynamic_pointer_cast<Slider>(shared_from_this()));
		}
	}

	double Slider::GetRawFromValue()
	{
		return 1.0 - GetValue();
	}

	void Slider::SetValueFromRaw(double raw)
	{
		SetValue(1.0 - raw);
	}
}