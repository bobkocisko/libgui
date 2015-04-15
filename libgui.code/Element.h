#pragma once 
#include "Structs.h"

using namespace std;

namespace libgui
{
	class Element : public enable_shared_from_this<Element>
	{
	public:
		virtual list<shared_ptr<Element>> GetChildren();

		void SetArrangedLocation(Location);
		void SetArrangedSize(Size);

		Location GetArrangedLocation();
		Size GetArrangedSize();

		virtual bool HitTest(Location);

	private:
		Location m_arrangedLocation = {};
		Size m_arrangedSize = {};
	};
}
