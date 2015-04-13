#pragma once 
#include "Structs.h"

using namespace std;

namespace libgui
{
	class Element 
	{
	public:
		virtual void Arrange() {};

		virtual bool HitTest(Location);

		void SetWidth(double);
		void SetHeight(double);

		// A Width of 0.0 means that it is not set
		double GetWidth();
		// A Height of 0.0 means that it is not set
		double GetHeight();

	protected:
		void SetArrangedLocation(Location);
		void SetArrangedSize(Size);

		Location GetArrangedLocation();
		Size GetArrangedSize();
	private:
		shared_ptr<Element> Parent = nullptr;

		double m_width = 0.0;
		double m_height = 0.0;

		Location m_arrangedLocation = {};
		Size m_arrangedSize = {};
	};
}
