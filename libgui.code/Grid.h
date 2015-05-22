#pragma once
#include "Element.h"
#include "Scrollbar.h"

namespace libgui
{
	class Grid : public Element, public ScrollDelegate
	{
	
	public:
		Grid();

		void Arrange() override;

		double GetCurrentOffsetPercent() override;
		double GetThumbSizePercent() override;
		void MoveToOffsetPercent(double offsetPercent) override;

		bool CanScroll();

		const int& GetColumns() const;
		void SetColumns(int columns);

		void SetCellHeight(double);
		double GetCellHeight();

		const double& GetTopPadding() const;
		const double& GetBottomPadding() const;
		void SetTopPadding(double topPadding);
		void SetBottomPadding(double bottomPadding);

		const function<shared_ptr<Element>()>& GetCellCreateCallback() const;
		void SetCellCreateCallback(const function<shared_ptr<Element>()>& cellCreateCallback);

		const function<int(shared_ptr<Element>)>& GetTotalCountCallback() const;
		void SetTotalCountCallback(const function<int(shared_ptr<Element>)>& totalCountCallback);

		const function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)>& GetCellViewModelCallback() const;
		void SetCellViewModelCallback(const function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)>& cellViewModelCallback);

	private:
		int m_columns = 3;
		double m_cellHeight;
		double m_cellWidth;
		double m_offsetPercent = 0.0;
		int m_baseItemIndex;
		double m_rowOffset;

		double m_topPadding = 0.0;
		double m_bottomPadding = 0.0;

		function<shared_ptr<Element>()> m_cellCreateCallback;
		function<int(shared_ptr<Element>)> m_totalCountCallback;
		function<shared_ptr<ViewModelBase>(shared_ptr<ViewModelBase>, int)> m_cellViewModelCallback;

		class Cell : public Element
		{
		public:
			Cell(const shared_ptr<Grid>& parent, int index);

			void PrepareViewModel() override;
			void Arrange() override;
		private:
			shared_ptr<Grid> m_grid;
			int m_index;
		};
	};
}

