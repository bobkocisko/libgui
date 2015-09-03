#pragma once 
#include "Structs.h"
#include "ViewModelBase.h"

using namespace std;

namespace libgui
{
#undef GetFirstChild
#undef GetPrevSibling
#undef GetNextSibling

	class ElementManager;

	class Element : public enable_shared_from_this < Element >
	{
	public:
		// Element Manager
		void SetElementManager(shared_ptr<ElementManager>);
		shared_ptr<ElementManager> GetElementManager();

		// View Model
		void SetViewModel(shared_ptr<ViewModelBase>);
		shared_ptr<ViewModelBase> GetViewModel();

		// Visual tree
		void RemoveChildren();
		void AddChild(shared_ptr<Element>);
		void SetSingleChild(shared_ptr<Element> child);
		int GetChildrenCount();

		// Cache Management
		virtual void ClearCache(int cacheLevel);
		virtual void ClearElementCache(int cacheLevel);

		// Arrangement
		shared_ptr<Element> GetParent();
		shared_ptr<Element> GetSingleChild();
		shared_ptr<Element> GetFirstChild();
		shared_ptr<Element> GetLastChild();
		shared_ptr<Element> GetPrevSibling();
		shared_ptr<Element> GetNextSibling();
		virtual void PrepareViewModel();
		virtual void Arrange();
		virtual void ResetArrangement();
		virtual void ArrangeAndDraw(bool draw);

		void SetSetViewModelCallback(function<void(shared_ptr<Element>)>);
		void SetArrangeCallback(function<void(shared_ptr<Element>)>);

		void SetIsVisible(bool isVisible);
		bool GetIsVisible();
		void SetClipToBounds(bool clipToBounds);
		bool GetClipToBounds();

		void SetLeft(double left);
		void SetTop(double top);
		void SetRight(double right);
		void SetBottom(double bottom);
		void SetCenterX(double centerX);
		void SetCenterY(double centerY);
		void SetWidth(double width);
		void SetHeight(double height);

		double GetLeft();
		double GetTop();
		double GetRight();
		double GetBottom();
		double GetCenterX();
		double GetCenterY();
		double GetWidth();
		double GetHeight();

		// Drawing
		virtual void Draw();
		void SetDrawCallback(function<void(shared_ptr<Element>)>);

		// Hit testing
		shared_ptr<Element> GetElementAtPoint(Location);

		virtual ~Element();

	private:
		// Element manager
		shared_ptr<ElementManager> m_elementManager;

		shared_ptr<ViewModelBase> m_viewModel;

		// Visual tree
		shared_ptr<Element> m_parent;
		shared_ptr<Element> m_firstChild;
		shared_ptr<Element> m_lastChild;
		shared_ptr<Element> m_prevsibling;
		shared_ptr<Element> m_nextsibling;
		int m_childrenCount = 0;

		// Arrangement
		function<void(shared_ptr<Element>)> m_setViewModelCallback;
		function<void(shared_ptr<Element>)> m_arrangeCallback;

		bool m_clipToBounds = false;
		bool m_isVisible = true;

		double m_left = 0;
		double m_top = 0;
		double m_right = 0;
		double m_bottom = 0;
		double m_centerX = 0;
		double m_centerY = 0;
		double m_width = 0;
		double m_height = 0;

		bool m_isLeftSet = false;
		bool m_isTopSet = false;
		bool m_isRightSet = false;
		bool m_isBottomSet = false;
		bool m_isCenterXSet = false;
		bool m_isCenterYSet = false;
		bool m_isWidthSet = false;
		bool m_isHeightSet = false;

		// Drawing
		function<void(shared_ptr<Element>)> m_drawCallback;
	};
}
