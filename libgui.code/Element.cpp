#include "Precompiled.h"
#include "Element.h"

namespace libgui
{
	list<shared_ptr<Element>> Element::GetChildren()
	{
		// By default there are no children
		return list<shared_ptr<Element>>();
	}

	void Element::SetArrangedLocation(Location location)
	{
		m_arrangedLocation = location;
	}

	void Element::SetArrangedSize(Size size)
	{
		m_arrangedSize = size;
	}

	Location Element::GetArrangedLocation()
	{
		return m_arrangedLocation;
	}
	Size Element::GetArrangedSize()
	{
		return m_arrangedSize;
	}

	bool Element::HitTest(Location point)
	{
		auto arrangedLocation = GetArrangedLocation();
		auto arrangedSize = GetArrangedSize();

		double left = arrangedLocation.x;
		double top = arrangedLocation.y;
		double right = arrangedLocation.x + arrangedSize.width;
		double bottom = arrangedLocation.y + arrangedSize.height;

		return point.x >= left && point.x <= right &&
			point.y >= top && point.y <= bottom;
	}
}
