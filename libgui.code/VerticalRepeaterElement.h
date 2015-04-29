#pragma once
#include "Element.h"

namespace libgui
{
	class VerticalRepeaterElement : public Element
	{
	public:
		void Arrange();

		void SetRowHeight(double);
		void SetRowCreateCallback(function<shared_ptr<Element>()>);
		void SetTotalCountCallback(function<int(shared_ptr<Element>)>);
		//void SetRowArrangeCallback(function<void(shared_ptr<Element>)>);
		//void SetRowDrawCallback(function<void(shared_ptr<Element>)>);

		double GetRowHeight();
		
	private:
		double m_rowHeight;
		function<shared_ptr<Element>()> m_rowCreateCallback;
		function<int(shared_ptr<Element>)> m_totalCountCallback;
		//function<void(shared_ptr<Element>)> m_rowArrangeCallback;
		//function<void(shared_ptr<Element>)> m_rowDrawCallback;

		class RowElement : public Element
		{
		public:
			RowElement(shared_ptr<VerticalRepeaterElement>);
			void Arrange();
		private:
			shared_ptr<VerticalRepeaterElement> m_parent;
		};
	};
}