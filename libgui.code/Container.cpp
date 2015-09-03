#include "include/Common.h"
#include "include/Container.h"

namespace libgui
{
	list<shared_ptr<Element>> Container::GetChildren()
	{
		// return a copy of the children list
		return list<shared_ptr<Element>>(m_children);
	}

	void Container::AddChild(shared_ptr<Element> element)
	{
		m_children.push_back(element);
	}

}