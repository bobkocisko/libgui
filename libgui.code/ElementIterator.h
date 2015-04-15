#pragma once
#include "Element.h"

namespace libgui
{
	class ElementIterator
	{
	public:
		static void FrontToBack(shared_ptr<Element> root, function<bool(shared_ptr<Element>)> action);
		static void BackToFront(shared_ptr<Element> root, function<bool(shared_ptr<Element>)> action);

	private:
		static bool FrontToBackHelper(shared_ptr<Element> root, function<bool(shared_ptr<Element>)> action);
		static bool BackToFrontHelper(shared_ptr<Element> root, function<bool(shared_ptr<Element>)> action);
	};
}