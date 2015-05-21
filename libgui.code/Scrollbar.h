#pragma once
#include "Control.h"
#include "ScrollDelegate.h"

namespace libgui
{
	class Scrollbar : public libgui::Element
	{
	public:
		explicit Scrollbar(const shared_ptr<ScrollDelegate>& scroll_info_provider);

		void Init();

		class Thumb;

		const shared_ptr<Thumb>& GetThumb() const;
		void SetThumb(const shared_ptr<Thumb>& thumb);

		const shared_ptr<ScrollDelegate>& GetScrollDelegate() const;

		void SetScrollDelegate(const shared_ptr<ScrollDelegate>& scroll_info_provider);

		class Thumb : public Control
		{
		public:
			explicit Thumb(weak_ptr<Scrollbar> scrollbar);

			void NotifyEnter() override;
			void NotifyLeave() override;
			void NotifyDown() override;
			void NotifyUp() override;

			const weak_ptr<Scrollbar>& GetScrollbar() const;

		private:
			weak_ptr<Scrollbar> scrollbar_;
		};
	private:
		shared_ptr<Thumb> thumb_;
		shared_ptr<ScrollDelegate> scroll_delegate_;
	};
}

