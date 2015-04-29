#include "Precompiled.h"
#include "VerticalRepeaterElement.h"

namespace libgui
{
	void VerticalRepeaterElement::Arrange()
	{
		// First perform arrangement for the repeater element itself
		Element::Arrange();

		// Then create children if necessary

		if (m_rowHeight == 0 || !m_totalCountCallback || !m_rowCreateCallback)
		{
			// Invalid state
			return;
		}

		auto totalCount = m_totalCountCallback();
		auto missingChildren = totalCount - GetChildrenCount();

		for (int i = 0; i < missingChildren; i++)
		{
			auto rowContainer = make_shared<RowElement>(dynamic_pointer_cast<VerticalRepeaterElement>(shared_from_this()));
			rowContainer->AddChild(m_rowCreateCallback());
			AddChild(rowContainer);
		}
	}

	VerticalRepeaterElement::RowElement::RowElement(shared_ptr<VerticalRepeaterElement> parent)
		:m_parent(parent)
	{ }

	void VerticalRepeaterElement::RowElement::Arrange()
	{
		auto ps = GetPrevSibling();

		SetLeft(m_parent->GetLeft());
		SetRight(m_parent->GetRight());

		SetHeight(m_parent->m_rowHeight);
		if (ps)
		{
			SetTop(ps->GetBottom());
		}
		else
		{
			SetTop(m_parent->GetTop());
		}
	}

	void VerticalRepeaterElement::SetRowHeight(double rowHeight)
	{
		m_rowHeight = rowHeight;
	}
	double VerticalRepeaterElement::GetRowHeight()
	{
		return m_rowHeight;
	}

	void VerticalRepeaterElement::SetRowCreateCallback(function<shared_ptr<Element>()> rowCreateCallback)
	{
		m_rowCreateCallback = rowCreateCallback;
	}
	void VerticalRepeaterElement::SetTotalCountCallback(function<int()> totalCountCallback)
	{
		m_totalCountCallback = totalCountCallback;
	}
	//void VerticalRepeaterElement::SetRowArrangeCallback(function<void(shared_ptr<Element>)> rowArrangeCallback)
	//{
	//	m_rowArrangeCallback = rowArrangeCallback;
	//}
	//void VerticalRepeaterElement::SetRowDrawCallback(function<void(shared_ptr<Element>)> rowDrawCallback)
	//{
	//	m_rowDrawCallback = rowDrawCallback;
	//}

}