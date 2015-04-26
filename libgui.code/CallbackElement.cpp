#include "Precompiled.h"
#include "CallbackElement.h"

namespace libgui
{
	CallbackElement::CallbackElement()
	{ }

	CallbackElement::CallbackElement(function<void(shared_ptr<Element>)> arrangeCallback)
		:CallbackElement(arrangeCallback, nullptr)
	{ }

	CallbackElement::CallbackElement(function<void(shared_ptr<Element>)> arrangeCallback, function<void(shared_ptr<Element>)> drawCallback)
		: m_arrangeCallback(arrangeCallback), m_drawCallback(drawCallback)
	{ }

	void CallbackElement::SetArrangeCallback(function<void(shared_ptr<Element>)> arrangeCallback)
	{
		m_arrangeCallback = arrangeCallback;
	}

	void CallbackElement::Arrange()
	{
		if (m_arrangeCallback)
		{
			m_arrangeCallback(shared_from_this());
		}
	}

	void CallbackElement::Draw()
	{
		if (m_drawCallback)
		{
			m_drawCallback(shared_from_this());
		}
	}

	void CallbackElement::SetDrawCallback(function<void(shared_ptr<Element>)> drawCallback)
	{
		m_drawCallback = drawCallback;
	}

}