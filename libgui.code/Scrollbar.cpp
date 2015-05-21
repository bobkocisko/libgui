#include "Precompiled.h"
#include "Scrollbar.h"

namespace libgui
{
	Scrollbar::Scrollbar(const shared_ptr<ScrollDelegate>& scroll_info_provider)
		: scroll_delegate_(scroll_info_provider)
	{
	}

	void Scrollbar::Init()
	{
		thumb_ = make_shared<Thumb>(
			dynamic_pointer_cast<Scrollbar>(shared_from_this()));
		this->AddChild(thumb_);
	}

	const shared_ptr<Scrollbar::Thumb>& Scrollbar::GetThumb() const
	{
		return thumb_;
	}

	void Scrollbar::SetThumb(const shared_ptr<Thumb>& thumb)
	{
		thumb_ = thumb;
	}

	const shared_ptr<ScrollDelegate>& Scrollbar::GetScrollDelegate() const
	{
		return scroll_delegate_;
	}

	void Scrollbar::SetScrollDelegate(const shared_ptr<ScrollDelegate>& scroll_delegate)
	{
		scroll_delegate_ = scroll_delegate;
	}

	Scrollbar::Thumb::Thumb(weak_ptr<Scrollbar> scrollbar)
		: scrollbar_(scrollbar)
	{
	}

	const weak_ptr<Scrollbar>& Scrollbar::Thumb::GetScrollbar() const
	{
		return scrollbar_;
	}
}