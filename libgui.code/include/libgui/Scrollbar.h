#pragma once
#include "Control.h"
#include "ScrollDelegate.h"
#include "Location.h"

namespace libgui
{
	class Scrollbar : public libgui::Element
	{
	public:
		explicit Scrollbar(const shared_ptr<ScrollDelegate>& scrollDelegate);

		virtual void Init();

		class Thumb;
		class Track;

		const shared_ptr<Thumb>& GetThumb() const;

		const shared_ptr<Track>& GetTrack() const;

		const shared_ptr<ScrollDelegate>& GetScrollDelegate() const;
		void SetScrollDelegate(const shared_ptr<ScrollDelegate>& scrollDelegate);

		class Track: public Element
		{
			
		};

		class Thumb : public Control
		{
		public:
			explicit Thumb(weak_ptr<Scrollbar> scrollbar, weak_ptr<Track> track);

			// Input events
			void NotifyMouseEnter() override;
			void NotifyMouseLeave() override;
			void NotifyMouseDown(Location location) override;
			void NotifyMouseUp(Location location) override;
			void NotifyMouseMove(Location location, bool& updateScreen) override;

			// Output states
			const bool& GetIsPressed() const;
			const bool& GetIsHot() const;

			void Arrange() override;

			const weak_ptr<Scrollbar>& GetScrollbar() const;

		private:
			weak_ptr<Scrollbar> m_scrollbar;
			weak_ptr<Track> m_track;
			bool m_isOver;
			bool m_isPressed;
			bool m_isHot;
			double m_anchorOffset;
		};
	private:
		shared_ptr<Thumb> m_thumb;
		shared_ptr<Track> m_track;
		shared_ptr<ScrollDelegate> m_scrollDelegate;
	};
}

