#include "Precompiled.h"
#include "ElementIterator.h"

namespace libgui
{
	void ElementIterator::FrontToBack(shared_ptr<Element> root, function<bool(shared_ptr<Element>)> action)
	{
		FrontToBackHelper(root, action);
	}

	bool ElementIterator::FrontToBackHelper(shared_ptr<Element> root, function<bool(shared_ptr<Element>)> action)
	{
		for (auto & child : root->GetChildren())
		{
			if (!FrontToBackHelper(child, action))
			{
				break;
			}
		}
		return action(root);
	}

	void ElementIterator::BackToFront(shared_ptr<Element> root, function<bool(shared_ptr<Element>)> action)
	{
		BackToFrontHelper(root, action);
	}

	bool ElementIterator::BackToFrontHelper(shared_ptr<Element> root, function<bool(shared_ptr<Element>)> action)
	{
		return action(root);
		for (auto & child : root->GetChildren())
		{
			if (!FrontToBackHelper(child, action))
			{
				break;
			}
		}
	}

}