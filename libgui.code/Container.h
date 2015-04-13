#pragma once
#include "Element.h"

namespace libgui
{
	class Container : public Element
	{
	public:
		void AddChild(shared_ptr<Element>);
		void Arrange();
	private:
		vector<shared_ptr<Element>> m_children;
	};
}