#pragma once
#include "ViewModelBase.h"

using namespace std;

namespace libgui
{
	class ItemsProvider
	{
	public:
		virtual ~ItemsProvider();

		virtual int GetTotalItems() = 0;
		virtual shared_ptr<ViewModelBase> GetItem(int index) = 0;
	};
}

