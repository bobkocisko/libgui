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
		void SetRowViewModelCallback(function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)>);

		double GetRowHeight();
		
	private:
		double m_rowHeight;
		function<shared_ptr<Element>()> m_rowCreateCallback;
		function<int(shared_ptr<Element>)> m_totalCountCallback;
		function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)> m_rowViewModelCallback;

		class RowElement : public Element
		{
		public:
			RowElement(shared_ptr<VerticalRepeaterElement>, int);
			void Arrange();
		private:
			shared_ptr<VerticalRepeaterElement> m_parent;
			int m_index;
		};
	};
}