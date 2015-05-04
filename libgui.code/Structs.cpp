#include "Precompiled.h"
#include "Structs.h"

namespace libgui
{
	Rect::Rect(Location location, Size size)
		:location(location),
		size(size)
	{
	}

	double Rect::GetLeft()
	{
		return location.x;
	}

	double Rect::GetTop()
	{
		return location.y;
	}

	double Rect::GetRight()
	{
		return location.x + size.width;
	}

	double Rect::GetBottom()
	{
		return location.y + size.height;
	}
}