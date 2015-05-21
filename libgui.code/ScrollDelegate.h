#pragma once

namespace libgui
{
	class ScrollDelegate
	{
	public:
		virtual ~ScrollDelegate() { }

		virtual double GetCurrentOffsetPercent() = 0;
		virtual double GetThumbSizePercent() = 0;

		virtual void MoveToOffsetPercent(double offset_percent) = 0;
	};
}

