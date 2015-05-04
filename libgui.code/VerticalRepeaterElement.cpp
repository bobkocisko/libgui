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

		auto currentChildrenCount = GetChildrenCount();
		auto totalCount = m_totalCountCallback(shared_from_this());
		auto missingChildren = totalCount - currentChildrenCount;

		for (int i = 0; i < missingChildren; i++)
		{
			auto rowContainer = make_shared<RowElement>(
				dynamic_pointer_cast<VerticalRepeaterElement>(shared_from_this()),
				currentChildrenCount + i);
			AddChild(rowContainer);
			rowContainer->AddChild(m_rowCreateCallback());
		}

		// TODO: support removing children to match viewmodel
	}

	VerticalRepeaterElement::RowElement::RowElement(shared_ptr<VerticalRepeaterElement> parent, int index)
		:m_parent(parent), m_index(index)
	{ }

	void VerticalRepeaterElement::RowElement::Arrange()
	{
		// Get the view model for this particular row's index
		if (m_parent->m_rowViewModelCallback)
		{
			SetViewModel(m_parent->m_rowViewModelCallback(m_parent->GetViewModel(), m_index));
		}

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
	void VerticalRepeaterElement::SetTotalCountCallback(function<int(shared_ptr<Element>)> totalCountCallback)
	{
		m_totalCountCallback = totalCountCallback;
	}
	void VerticalRepeaterElement::SetRowViewModelCallback(function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)> rowViewModelCallback)
	{
		m_rowViewModelCallback = rowViewModelCallback;
	}

}