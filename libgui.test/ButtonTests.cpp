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

		TEST_METHOD(WhenMouseEntering_StateIsHot)
		{
			auto btn = make_shared<Button>();
			btn->NotifyMouseEnter();

			Assert::AreEqual(true, btn->IsHot());
		}

		TEST_METHOD(WhenMouseLeaving_StateIsNotHot)
		{
			auto btn = make_shared<Button>();
			btn->NotifyMouseEnter();
			btn->NotifyMouseLeave();
			Assert::AreEqual(false, btn->IsHot());
		}

		TEST_METHOD(WhenMouseDowning_StateIsHotAndPressed)
		{
			auto em = make_shared<ElementManager>();
			auto btn = make_shared<Button>();
			em->SetRoot(btn);
			btn->NotifyMouseDown(Location());
			Assert::AreEqual(true, btn->IsHot());
			Assert::AreEqual(true, btn->IsPressed());
		}

		TEST_METHOD(WhenMouseUpping_StateIsNotPressed)
		{
			auto btn = make_shared<Button>();
			btn->NotifyMouseUp(Location());
			Assert::AreEqual(false, btn->IsPressed());
		}

		TEST_METHOD(WhenMouseDownAndLeaving_StateIsHotAndNotPressed)
		{
			auto em = make_shared<ElementManager>();
			auto btn = make_shared<Button>();
			em->SetRoot(btn);
			btn->NotifyMouseDown(Location());
			btn->NotifyMouseLeave();
			Assert::AreEqual(true, btn->IsHot());
			Assert::AreEqual(false, btn->IsPressed());
		}

		TEST_METHOD(WhenMouseDownAndReturning_StateIsHotAndPressed)
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

		TEST_METHOD(WhenMouseLeftAndUppingOutside_StateIsNothing)
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

		TEST_METHOD(WhenMouseLeftAndUppingOutside_NoClick)
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

		TEST_METHOD(AfterMouseClicked_StateIsHot)
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

		TEST_METHOD(WhenTouchDownAndUp_Click)
		{
			auto em = make_shared<ElementManager>();
			auto btn = make_shared<Button>();
			bool isClicked = false;
			btn->SetClickCallback([&](shared_ptr<Button> b)
			{
				isClicked = true;
			});

			em->SetRoot(btn);
			btn->NotifyTouchDown(Location());
			btn->NotifyTouchUp(Location());
			Assert::AreEqual(true, isClicked);
		}

		TEST_METHOD(WhenTouchDownLeaveReturnAndUp_StateIsNotPressed)
		{
			auto em = make_shared<ElementManager>();
			auto btn = make_shared<Button>();

			em->SetRoot(btn);
			btn->NotifyTouchDown(Location());
			btn->NotifyTouchLeave();
			btn->NotifyTouchEnter();
			btn->NotifyTouchUp(Location());

			Assert::AreEqual(false, btn->IsPressed());
			Assert::AreEqual(false, btn->IsHot());
		}
	};
}