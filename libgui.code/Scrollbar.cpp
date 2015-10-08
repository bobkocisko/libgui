#include "libgui/Common.h"
#include "libgui/Scrollbar.h"
#include "libgui/ElementManager.h"
#include "libgui/Location.h"

namespace libgui
{
	Scrollbar::Scrollbar(const shared_ptr<ScrollDelegate>& scrollDelegate)
		: m_scrollDelegate(scrollDelegate)
	{
	}

	void Scrollbar::Init()
	{
		m_track = make_shared<Track>();
		this->AddChild(m_track);

		m_thumb = make_shared<Thumb>(
			dynamic_pointer_cast<Scrollbar>(shared_from_this()),
			m_track);
		m_track->AddChild(m_thumb);
	}

	const shared_ptr<Scrollbar::Thumb>& Scrollbar::GetThumb() const
	{
		return m_thumb;
	}

	const shared_ptr<Scrollbar::Track>& Scrollbar::GetTrack() const
	{
		return m_track;
	}

	const shared_ptr<ScrollDelegate>& Scrollbar::GetScrollDelegate() const
	{
		return m_scrollDelegate;
	}

	void Scrollbar::SetScrollDelegate(const shared_ptr<ScrollDelegate>& scrollDelegate)
	{
		m_scrollDelegate = scrollDelegate;
	}

	Scrollbar::Thumb::Thumb(weak_ptr<Scrollbar> scrollbar, weak_ptr<Track> track)
		: m_scrollbar(scrollbar),
		m_track(track)
	{
	}

	void Scrollbar::Thumb::Arrange()
	{
		if (auto sb = m_scrollbar.lock())
		{
			auto scrollDelegate = sb->GetScrollDelegate();

			auto p = GetParent();
			SetLeft(p->GetLeft()); SetRight(p->GetRight());
			auto trackHeight = p->GetHeight();
			auto height = scrollDelegate->GetThumbSizePercent() * trackHeight;
			auto top = p->GetTop() + (scrollDelegate->GetCurrentOffsetPercent() * trackHeight);
			SetTop(round(top)); SetBottom(round(top + height));
		}
	}

	void Scrollbar::Thumb::NotifyMouseEnter()
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

	void Scrollbar::Thumb::NotifyMouseLeave()
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

	void Scrollbar::Thumb::NotifyMouseDown(Location location)
	{
		m_isHot = true;
		m_isPressed = true;

		m_anchorOffset = location.y - GetTop();

		GetElementManager()->RequestCapture(dynamic_pointer_cast<Control>(shared_from_this()));
	}

	void Scrollbar::Thumb::NotifyMouseUp(Location location)
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

	void Scrollbar::Thumb::NotifyMouseMove(Location location, bool& updateScreen)
	{
		if (IsCapturing())
		{
			auto sb = m_scrollbar.lock();
			auto track = m_track.lock();
			if (sb && track)
			{
				auto scrollDelegate = sb->GetScrollDelegate();
				auto offsetPercent = ((location.y - m_anchorOffset) - track->GetTop()) / track->GetHeight();
				scrollDelegate->LimitToBounds(offsetPercent);
				if (scrollDelegate->GetCurrentOffsetPercent() != offsetPercent)
				{
					scrollDelegate->MoveToOffsetPercent(offsetPercent);
					updateScreen = true;
					return;
				}
			}
		}
		updateScreen = false;
		return;
	}

	const bool& Scrollbar::Thumb::GetIsPressed() const
	{
		return m_isPressed;
	}

	const bool& Scrollbar::Thumb::GetIsHot() const
	{
		return m_isHot;
	}

	const weak_ptr<Scrollbar>& Scrollbar::Thumb::GetScrollbar() const
	{
		return m_scrollbar;
	}
}