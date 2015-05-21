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
		double row_height_;
		function<shared_ptr<Element>()> row_create_callback_;
		function<int(shared_ptr<Element>)> total_count_callback_;
		function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)> row_view_model_callback_;

		class RowElement : public Element
		{
		public:
			RowElement(shared_ptr<VerticalRepeaterElement>, int);
			void Arrange();
		private:
			shared_ptr<VerticalRepeaterElement> parent_;
			int index_;
		};
	};
}