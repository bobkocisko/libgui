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

			Assert::AreEqual(true, sc->GetNotifyUpCalled());
		}


	private:
		class StubControl: public Control
		{
		public:
			void NotifyDown(Location location) override
			{
				GetElementManager()->RequestCapture(dynamic_pointer_cast<Control>(shared_from_this()));
			}

			void NotifyUp(Location location) override
			{
				notify_up_called_ = true;
			}

			const bool& GetNotifyUpCalled() const
			{
				return notify_up_called_;
			}

		private:
			bool notify_up_called_ = false;
		};
	};
}