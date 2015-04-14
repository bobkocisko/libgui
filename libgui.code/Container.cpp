#include "Precompiled.h"
#include "Container.h"

namespace libgui
{
	shared_ptr<Element> Container::ElementAtPoint(Location point)
	{
		// First check if any children are hit
		for (auto &element : m_children)
		{
			auto& elementAtPoint = element->ElementAtPoint(point);
			if (elementAtPoint)
			{
				return elementAtPoint;
			}
		}

		// Then find out if the container itself is hit
		return Element::ElementAtPoint(point);
	}

	void Container::Arrange()
	{
		// First arrange this container
		Element::Arrange();

		// Then arrange all the children
		for (auto& element : m_children)
		{
			element->Arrange();
		}
	}

	void Container::AddChild(shared_ptr<Element> element)
	{
		m_children.push_back(element);
	}

}