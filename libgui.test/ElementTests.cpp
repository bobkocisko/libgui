#include "Precompiled.h"
#include "CppUnitTest.h"
#include <Element.h>
#include <ElementManager.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace libgui;

namespace libguitest
{
	TEST_CLASS(ElementTests)
	{
	public:
		
		TEST_METHOD(WhenRemovingChildren_AllReferencesAreCleaned)
		{
			auto root = make_shared<Element>();
			auto em = make_shared<ElementManager>();
			em->SetRoot(root);

			auto child1 = make_shared<Element>();
			root->AddChild(child1);

			auto child2 = make_shared<TestElement>();
			root->AddChild(child2);
			bool wasDestructed = false;
			child2->SetDestructorCallback([&]() { wasDestructed = true; });

			auto grandchild1 = make_shared<Element>();
			child2->AddChild(grandchild1);
			auto grandchild2 = make_shared<Element>();
			child2->AddChild(grandchild2);
			auto grandchild3 = make_shared<Element>();
			child2->AddChild(grandchild3);

			root->RemoveChildren();

			child2 = nullptr;

			Assert::AreEqual(true, wasDestructed);
		}

	private:
		class TestElement: public Element
		{
		public:
			~TestElement()
			{
				if (destructor_callback_)
				{
					destructor_callback_();
				}
			}

			void SetDestructorCallback(const function<void()>& destructor_callback)
			{
				destructor_callback_ = destructor_callback;
			}

		private:
			function<void()> destructor_callback_;
		};

	};
}