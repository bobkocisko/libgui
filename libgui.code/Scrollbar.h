#pragma once
#include "Control.h"
#include "ScrollDelegate.h"

namespace libgui
{
	class Scrollbar : public libgui::Element
	{
	public:
		explicit Scrollbar(const shared_ptr<ScrollDelegate>& scrollDelegate);

		void Init();

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
			void NotifyEnter() override;
			void NotifyLeave() override;
			void NotifyDown(Location location) override;
			void NotifyUp(Location location) override;
			bool NotifyMove(Location location) override;

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

