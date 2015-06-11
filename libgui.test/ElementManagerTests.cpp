#include "Precompiled.h"
#include "CppUnitTest.h"
#include <ElementManager.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace libgui;

namespace libguitest
{
	TEST_CLASS(ElementManagerTests)
	{
	public:
		
		TEST_METHOD(WhenControlIsCaptured_ItReceivesNotifyUp)
		{
			auto em = make_shared<ElementManager>();
			auto sc = make_shared<StubControl>();
			em->SetRoot(sc);
			sc->SetLeft(1); sc->SetRight(2);
			sc->SetTop(1); sc->SetBottom(2);

			em->NotifyMouseMove(1, 1);
			em->NotifyMouseDown(1, 1);
			em->NotifyMouseMove(0, 0);
			em->NotifyMouseUp(0, 0);

			Assert::AreEqual(true, sc->GetNotifyMouseUpCalled());
		}

		TEST_METHOD(WhenControlIsTouchedDownAndUp_ItReceivesNotifications)
		{
			auto em = make_shared<ElementManager>();
			auto sc = make_shared<StubControl>();
			em->SetRoot(sc);
			sc->SetLeft(1); sc->SetRight(2);
			sc->SetTop(1); sc->SetBottom(2);

			em->NotifyTouchDown(1, 1);
			em->NotifyTouchUp(1, 1);

			Assert::AreEqual(true, sc->GetNotifyTouchDownCalled());
			Assert::AreEqual(true, sc->GetNotifyTouchUpCalled());
		}

		TEST_METHOD(WhenControlIsTouchedDownLeavesAndReturns_ItReceivesNotifyEnter)
		{
			auto em = make_shared<ElementManager>();
			auto sc = make_shared<StubControl>();
			em->SetRoot(sc);
			sc->SetLeft(1); sc->SetRight(2);
			sc->SetTop(1); sc->SetBottom(2);

			em->NotifyTouchDown(1, 1);
			em->NotifyTouchMove(0, 0);
			em->NotifyTouchMove(1, 1);

			Assert::AreEqual(true, sc->GetNotifyTouchDownCalled());
			Assert::AreEqual(true, sc->GetNotifyTouchEnterCalled());
		}


	private:
		class StubControl: public Control
		{
		public:
			void NotifyMouseDown(Location location) override
			{
				GetElementManager()->RequestCapture(dynamic_pointer_cast<Control>(shared_from_this()));
			}

			void NotifyMouseUp(Location location) override
			{
				m_notifyMouseUpCalled = true;
			}

			const bool& GetNotifyMouseUpCalled() const
			{
				return m_notifyMouseUpCalled;
			}

			void NotifyTouchDown(Location location) override
			{
				m_notifyTouchDownCalled = true;
				GetElementManager()->RequestCapture(dynamic_pointer_cast<Control>(shared_from_this()));
			}

			void NotifyTouchEnter() override
			{
				m_notifyTouchEnterCalled = true;
			}

			const bool& GetNotifyTouchDownCalled() const
			{
				return m_notifyTouchDownCalled;
			}

			void NotifyTouchUp(Location location) override
			{
				m_notifyTouchUpCalled = true;
			}

			const bool& GetNotifyTouchUpCalled() const
			{
				return m_notifyTouchUpCalled;
			}

			const bool& GetNotifyTouchEnterCalled() const
			{
				return m_notifyTouchEnterCalled;
			}

		private:
			bool m_notifyMouseUpCalled = false;

			bool m_notifyTouchDownCalled = false;
			bool m_notifyTouchUpCalled = false;
			bool m_notifyTouchEnterCalled = false;
		};
	};
}