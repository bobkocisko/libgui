#pragma once
#include "Element.h"

namespace libgui
{
	class Grid : public Element
	{
	public:
		void Arrange() override;

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

	private:
		int columns_ = 3;
		double cell_height_;
		double cell_width_;
		function<shared_ptr<Element>()> cell_create_callback_;
		function<int(shared_ptr<Element>)> total_count_callback_;
		function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)> cell_view_model_callback_;

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

