#include "Precompiled.h"
#include "Container.h"

namespace libgui
{
	void Container::Arrange()
	{
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