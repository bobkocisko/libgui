#include "Precompiled.h"
#include "Element.h"
#include "ElementManager.h"

namespace libgui
{
	// Element Manager
	void Element::SetElementManager(shared_ptr<ElementManager> elementManager)
	{
		m_elementManager = elementManager;
	}

	shared_ptr<ElementManager> Element::GetElementManager()
	{
		return m_elementManager;
	}

	// View Model
	void Element::SetViewModel(shared_ptr<ViewModelBase> viewModel)
	{
		m_viewModel = viewModel;
	}

	shared_ptr<ViewModelBase> Element::GetViewModel()
	{
		return m_viewModel;
	}

	// Visual tree
	shared_ptr<Element> Element::GetParent()
	{
		return m_parent;
	}

	shared_ptr<Element> Element::GetFirstChild()
	{
		return m_firstChild;
	}

	shared_ptr<Element> Element::GetLastChild()
	{
		return m_lastChild;
	}

	shared_ptr<Element> Element::GetPrevSibling()
	{
		return m_prevsibling;
	}
	shared_ptr<Element> Element::GetNextSibling()
	{
		return m_nextsibling;
	}

	void Element::RemoveChildren()
	{
		m_firstChild = nullptr;
		m_lastChild = nullptr;
		m_childrenCount = 0;
	}

	void Element::AddChild(shared_ptr<Element> element)
	{
		if (m_firstChild == nullptr)
		{
			m_firstChild = element;
		}
		else
		{
			element->m_prevsibling = m_lastChild;
			m_lastChild->m_nextsibling = element;
		}

		element->m_parent = shared_from_this();
		m_lastChild = element;

		m_childrenCount++;
	}

	int Element::GetChildrenCount()
	{
		return m_childrenCount;
	}

	void Element::SetSingleChild(shared_ptr<Element> child)
	{
		// Could be optimized to improve performance
		RemoveChildren();
		AddChild(child);
	}

	shared_ptr<Element> Element::GetSingleChild()
	{
		if (m_childrenCount == 1)
		{
			return m_firstChild;
		}

		if (m_childrenCount == 0)
		{
			return nullptr;
		}

		throw std::runtime_error("There is more than one child in this element");
	}

	// Arrangement
	void Element::ResetArrangement()
	{
		if (m_parent)
		{
			// Copy the element manager from the parent
			m_elementManager = m_parent->m_elementManager;
		}

		m_isVisible = true;

		m_left = 0;
		m_top = 0;
		m_right = 0;
		m_bottom = 0;
		m_centerX = 0;
		m_centerY = 0;
		m_width = 0;
		m_height = 0;

		m_isLeftSet = false;
		m_isTopSet = false;
		m_isRightSet = false;
		m_isBottomSet = false;
		m_isCenterXSet = false;
		m_isCenterYSet = false;
		m_isWidthSet = false;
		m_isHeightSet = false;
	}

	void Element::SetSetViewModelCallback(function<void(shared_ptr<Element>)> setViewModelCallback)
	{
		m_setViewModelCallback = setViewModelCallback;
	}

	void Element::PrepareViewModel()
	{
		if (m_setViewModelCallback)
		{
			m_setViewModelCallback(shared_from_this());
		}
		else
		{
			// By default the ViewModel is copied from the parent
			if (m_parent)
			{
				m_viewModel = m_parent->m_viewModel;
			}
		}
	}

	void Element::SetArrangeCallback(function<void(shared_ptr<Element>)> arrangeCallback)
	{
		m_arrangeCallback = arrangeCallback;
	}

	void Element::Arrange()
	{
		if (m_arrangeCallback)
		{
			m_arrangeCallback(shared_from_this());
		}
		else
		{
			// By default each element stretches
			// to fill its parent
			SetLeft(m_parent->GetLeft());
			SetTop(m_parent->GetTop());
			SetRight(m_parent->GetRight());
			SetBottom(m_parent->GetBottom());
		}
	}

	void Element::ArrangeAndDraw(bool draw)
	{
		ResetArrangement();
		PrepareViewModel();
		Arrange();
		if (draw && m_isVisible)
		{
			Draw();
			if (m_firstChild)
			{
				for (auto e = m_firstChild; e != nullptr; e = e->m_nextsibling)
				{
					e->ArrangeAndDraw(draw);
				}
			}
		}
	}

	void Element::SetIsVisible(bool isVisible)
	{
		m_isVisible = isVisible;
	}

	bool Element::GetIsVisible()
	{
		return m_isVisible;
	}


	void Element::SetLeft(double left)
	{
		m_isLeftSet = true;
		m_left = left;
	}

	void Element::SetTop(double top)
	{
		m_isTopSet = true;
		m_top = top;
	}

	void Element::SetRight(double right)
	{
		m_isRightSet = true;
		m_right = right;
	}

	void Element::SetBottom(double bottom)
	{
		m_isBottomSet = true;
		m_bottom = bottom;
	}

	void Element::SetCenterX(double centerX)
	{
		m_isCenterXSet = true;
		m_centerX = centerX;
	}

	void Element::SetCenterY(double centerY)
	{
		m_isCenterYSet = true;
		m_centerY = centerY;
	}

	void Element::SetWidth(double width)
	{
		m_isWidthSet = true;
		m_width = width;
	}

	void Element::SetHeight(double height)
	{
		m_isHeightSet = true;
		m_height = height;
	}

	double Element::GetLeft()
	{
		if (!m_isLeftSet)
		{
			if (m_isWidthSet)
			{
				if (m_isRightSet)
				{
					m_left = m_right - m_width;
				}
				else if (m_isCenterXSet)
				{
					m_left = m_centerX - (m_width / 2);
				}
			}
			m_isLeftSet = true;
		}
		return m_left;
	}

	double Element::GetTop()
	{
		if (!m_isTopSet)
		{
			if (m_isHeightSet)
			{
				if (m_isBottomSet)
				{
					m_top = m_bottom - m_height;
				}
				else if (m_isCenterYSet)
				{
					m_top = m_centerY - (m_height / 2);
				}
			}
			m_isTopSet = true;
		}
		return m_top;
	}

	double Element::GetRight()
	{
		if (!m_isRightSet)
		{
			if (m_isWidthSet)
			{
				if (m_isLeftSet)
				{
					m_right = m_left + m_width;
				}
				else if (m_isCenterXSet)
				{
					m_right = m_centerX + (m_width / 2);
				}
			}
			m_isRightSet = true;
		}
		return m_right;
	}

	double Element::GetBottom()
	{
		if (!m_isBottomSet)
		{
			if (m_isHeightSet)
			{
				if (m_isTopSet)
				{
					m_bottom = m_top + m_height;
				}
				else if (m_isCenterYSet)
				{
					m_bottom = m_centerY + (m_height / 2);
				}
			}
			m_isBottomSet = true;
		}
		return m_bottom;
	}

	double Element::GetCenterX()
	{
		if (!m_isCenterXSet)
		{
			if (m_isLeftSet && m_isRightSet)
			{
				m_centerX = m_left + (m_right - m_left) / 2;
			}
			else if (m_isLeftSet && m_isWidthSet)
			{
				m_centerX = m_left + (m_width / 2);
			}
			else if (m_isRightSet && m_isWidthSet)
			{
				m_centerX = m_right - (m_width / 2);
			}
			m_isCenterXSet = true;
		}
		return m_centerX;
	}

	double Element::GetCenterY()
	{
		if (!m_isCenterYSet)
		{
			if (m_isTopSet && m_isBottomSet)
			{
				m_centerY = m_top + (m_bottom - m_top) / 2;
			}
			else if (m_isTopSet && m_isHeightSet)
			{
				m_centerY = m_top + (m_height / 2);
			}
			else if (m_isBottomSet && m_isHeightSet)
			{
				m_centerY = m_bottom - (m_height / 2);
			}
			m_isCenterYSet = true;
		}
		return m_centerY;
	}

	double Element::GetWidth()
	{
		if (!m_isWidthSet)
		{
			if (m_isLeftSet && m_isRightSet)
			{
				m_width = m_right - m_left;
			}
			m_isWidthSet = true;
		}
		return m_width;
	}

	double Element::GetHeight()
	{
		if (!m_isHeightSet)
		{
			if (m_isTopSet && m_isBottomSet)
			{
				m_height = m_bottom - m_top;
			}
			m_isHeightSet = true;
		}
		return m_height;
	}


	// Drawing
	void Element::Draw()
	{
		if (m_drawCallback)
		{
			m_drawCallback(shared_from_this());
		}
		else 
		{
			// By default no drawing takes place
		}
	}

	void Element::SetDrawCallback(function<void(shared_ptr<Element>)> drawCallback)
	{
		m_drawCallback = drawCallback;
	}

	// Hit testing
	shared_ptr<Element> Element::GetElementAtPoint(Location point)
	{
		if (m_firstChild)
		{
			for (auto e = m_lastChild; e != nullptr; e = e->m_prevsibling)
			{
				auto elementAtPoint = e->GetElementAtPoint(point);
				if (elementAtPoint)
				{
					return elementAtPoint;
				}
			}
		}

		if (point.x >= GetLeft() && point.x <= GetRight() &&
			point.y >= GetTop() && point.y <= GetBottom())
		{
			return shared_from_this();
		}
		return nullptr;
	}
}
