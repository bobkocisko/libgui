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
		Rect(double, double, double, double);

		Location location;
		Size size;

		double GetLeft();
		double GetTop();
		double GetRight();
		double GetBottom();
	};
}