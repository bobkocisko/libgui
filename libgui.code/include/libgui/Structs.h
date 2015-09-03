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
		Rect(Location, Size);

		Location location;
		Size size;

		double GetLeft();
		double GetTop();
		double GetRight();
		double GetBottom();
	};

	struct Rect4
	{
		Rect4(double left, double top, double right, double bottom);

		double left;
		double top;
		double right;
		double bottom;
	};
}