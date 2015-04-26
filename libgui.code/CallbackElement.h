#pragma once
#include "Element.h"

namespace libgui
{
	class CallbackElement : public Element
	{
	public:
		CallbackElement();
		CallbackElement(function<void(shared_ptr<Element>)> arrangeCallback);
		CallbackElement(function<void(shared_ptr<Element>)> arrangeCallback, function<void(shared_ptr<Element>)> drawCallback);

		// Arrangement
		void Arrange();
		void SetArrangeCallback(function<void(shared_ptr<Element>)>);

		// Drawing
		void Draw();
		void SetDrawCallback(function<void(shared_ptr<Element>)>);

	private:
		function<void(shared_ptr<Element>)> m_arrangeCallback;
		function<void(shared_ptr<Element>)> m_drawCallback;
	};
}