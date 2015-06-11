#include "Precompiled.h"
#include "CppUnitTest.h"
#include "Button.h"
#include "ElementManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace libgui;
using namespace std;

namespace libguitest
{
	TEST_CLASS(ButtonTests)
	{
	public:

		TEST_METHOD(WhenEntering_StateIsHot)
		{
			auto btn = make_shared<Button>();
			btn->NotifyMouseEnter();

			Assert::AreEqual(true, btn->IsHot());
		}

		TEST_METHOD(WhenLeaving_StateIsNotHot)
		{
			auto btn = make_shared<Button>();
			btn->NotifyMouseEnter();
			btn->NotifyMouseLeave();
			Assert::AreEqual(false, btn->IsHot());
		}

		TEST_METHOD(WhenDowning_StateIsHotAndPressed)
		{
			auto em = make_shared<ElementManager>();
			auto btn = make_shared<Button>();
			em->SetRoot(btn);
			btn->NotifyMouseDown(Location());
			Assert::AreEqual(true, btn->IsHot());
			Assert::AreEqual(true, btn->IsPressed());
		}

		TEST_METHOD(WhenUpping_StateIsNotPressed)
		{
			auto btn = make_shared<Button>();
			btn->NotifyMouseUp(Location());
			Assert::AreEqual(false, btn->IsPressed());
		}

		TEST_METHOD(WhenDownAndLeaving_StateIsHotAndNotPressed)
		{
			auto em = make_shared<ElementManager>();
			auto btn = make_shared<Button>();
			em->SetRoot(btn);
			btn->NotifyMouseDown(Location());
			btn->NotifyMouseLeave();
			Assert::AreEqual(true, btn->IsHot());
			Assert::AreEqual(false, btn->IsPressed());
		}

		TEST_METHOD(WhenDownAndReturning_StateIsHotAndPressed)
		{
			auto em = make_shared<ElementManager>();
			auto btn = make_shared<Button>();
			em->SetRoot(btn);
			btn->NotifyMouseDown(Location());
			btn->NotifyMouseLeave();
			btn->NotifyMouseEnter();
			Assert::AreEqual(true, btn->IsHot());
			Assert::AreEqual(true, btn->IsPressed());
		}

		TEST_METHOD(WhenLeftAndUppingOutside_StateIsNothing)
		{
			auto em = make_shared<ElementManager>();
			auto btn = make_shared<Button>();
			em->SetRoot(btn);
			btn->NotifyMouseEnter();
			btn->NotifyMouseDown(Location());
			btn->NotifyMouseLeave();
			btn->NotifyMouseUp(Location());
			Assert::AreEqual(false, btn->IsHot());
			Assert::AreEqual(false, btn->IsPressed());
		}

		TEST_METHOD(WhenLeftAndUppingOutside_NoClick)
		{
			auto em = make_shared<ElementManager>();
			auto btn = make_shared<Button>();
			bool is_clicked = false;
			btn->SetClickCallback([&](shared_ptr<Button> b)
			{
				is_clicked = true;
			});

			em->SetRoot(btn);
			btn->NotifyMouseEnter();
			btn->NotifyMouseDown(Location());
			btn->NotifyMouseLeave();
			btn->NotifyMouseUp(Location());
			Assert::AreEqual(false, is_clicked);
		}

		TEST_METHOD(AfterClicked_StateIsHot)
		{
			auto em = make_shared<ElementManager>();
			auto btn = make_shared<Button>();
			em->SetRoot(btn);
			btn->NotifyMouseEnter();
			btn->NotifyMouseDown(Location());
			btn->NotifyMouseUp(Location());
			Assert::AreEqual(true, btn->IsHot());
			Assert::AreEqual(false, btn->IsPressed());
		}
	};
}