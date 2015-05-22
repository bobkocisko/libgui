#pragma once
#include "Element.h"
#include "Scrollbar.h"

namespace libgui
{
	class Grid : public Element, public ScrollDelegate
	{
	
	public:
		void Arrange() override;

		double GetCurrentOffsetPercent() override;
		double GetThumbSizePercent() override;
		void MoveToOffsetPercent(double offset_percent) override;

		bool CanScroll();

		const int& GetColumns() const;
		void SetColumns(int columns);

		void SetCellHeight(double);
		double GetCellHeight();

		const function<shared_ptr<Element>()>& GetCellCreateCallback() const;
		void SetCellCreateCallback(const function<shared_ptr<Element>()>& cell_create_callback);

		const function<int(shared_ptr<Element>)>& GetTotalCountCallback() const;
		void SetTotalCountCallback(const function<int(shared_ptr<Element>)>& total_count_callback);

		const function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)>& GetCellViewModelCallback() const;
		void SetCellViewModelCallback(const function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)>& cell_view_model_callback);

		const shared_ptr<Scrollbar>& GetScrollbar() const;
		void SetScrollbar(const shared_ptr<Scrollbar>& scrollbar);

	private:
		int columns_ = 3;
		double cell_height_;
		double cell_width_;
		double offset_percent_ = 0;
		function<shared_ptr<Element>()> cell_create_callback_;
		function<int(shared_ptr<Element>)> total_count_callback_;
		function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)> cell_view_model_callback_;
		shared_ptr<Scrollbar> scrollbar_;

		class Cell : public Element
		{
		public:
			Cell(const shared_ptr<Grid>& parent, int index);

			void PrepareViewModel() override;
			void Arrange();
		private:
			shared_ptr<Grid> grid_;
			int index_;
		};
	};
}

