#pragma once
#include "Element.h"

namespace libgui
{
	class Container : public Element
	{
	public:
		void AddChild(shared_ptr<Element>);
		virtual void Arrange();
		virtual shared_ptr<Element> ElementAtPoint(Location);
	private:
		vector<shared_ptr<Element>> m_children;
	};
}