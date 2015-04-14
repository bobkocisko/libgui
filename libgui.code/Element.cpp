#include "Precompiled.h"
#include "Element.h"

namespace libgui
{
	shared_ptr<Element> Element::ElementAtPoint(Location point)
	{
		auto arrangedLocation = GetArrangedLocation();
		auto arrangedSize = GetArrangedSize();

		double left = arrangedLocation.x;
		double top = arrangedLocation.y;
		double right = arrangedLocation.x + arrangedSize.width;
		double bottom = arrangedLocation.y + arrangedSize.height;

		if (point.x >= left && point.x <= right &&
			point.y >= top && point.y <= bottom)
		{
			return shared_from_this();
		}

		return nullptr;
	}

	void Element::SetWidth(double width)
	{
		m_width = width;
	}
	void Element::SetHeight(double height)
	{
		m_height = height;
	}

	double Element::GetWidth()
	{
		return m_width;
	}
	double Element::GetHeight()
	{
		return m_height;
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

	void Element::SetArrangeCallback(function<void(shared_ptr<Element>)> f_arrange)
	{
		m_f_arrange = f_arrange;
	}

	function<void(shared_ptr<Element>)> Element::GetArrangeCallback()
	{
		return m_f_arrange;
	}

	void Element::Arrange()
	{
		// Call the arrange function if it has
		// been specified
		if (m_f_arrange)
		{
			m_f_arrange(shared_from_this());
		}
	}
}
