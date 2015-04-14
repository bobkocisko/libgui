#pragma once 
#include "Structs.h"

using namespace std;

namespace libgui
{
	class Element : public enable_shared_from_this<Element>
	{
	public:
		virtual void Arrange();

		void SetArrangeCallback(function<void(shared_ptr<Element>)>);
		void SetArrangedLocation(Location);
		void SetArrangedSize(Size);

		function<void(shared_ptr<Element>)> GetArrangeCallback();
		Location GetArrangedLocation();
		Size GetArrangedSize();

		virtual shared_ptr<Element> ElementAtPoint(Location);

		void SetWidth(double);
		void SetHeight(double);

		// A Width of 0.0 means that it is not set
		double GetWidth();
		// A Height of 0.0 means that it is not set
		double GetHeight();

	protected:
	private:
		shared_ptr<Element> Parent = nullptr;

		double m_width = 0.0;
		double m_height = 0.0;

		function<void(shared_ptr<Element>)> m_f_arrange = nullptr;

		Location m_arrangedLocation = {};
		Size m_arrangedSize = {};
	};
}
