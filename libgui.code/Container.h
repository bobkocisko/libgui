#pragma once
#include "Element.h"

namespace libgui
{
	class Container : public Element
	{
	public:
		virtual list<shared_ptr<Element>> GetChildren();
		void AddChild(shared_ptr<Element>);
	private:
		list<shared_ptr<Element>> m_children;
	};
}