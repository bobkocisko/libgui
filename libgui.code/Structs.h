#pragma once

namespace libgui
{
	struct Size
	{
		double width;
		double height;
	};

	struct Location
	{
		double x;
		double y;
	};

	struct Rect
	{
		Location location;
		Size size;
	};
}