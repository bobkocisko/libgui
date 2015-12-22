#include <gtest/gtest.h>
#include "include/Common.h"
#include "libgui/ElementManager.h"
#include "libgui/Location.h"

using namespace std;
using namespace libgui;

class StubControl: public Control
{
public:
    const bool& GetNotifyPointerPushCalled() const
    {
        return m_notifyPointerPushCalled;
    }

    const bool& GetNotifyPointerReleaseCalled() const
    {
        return m_notifyPointerReleaseCalled;
    }

    const bool& GetNotifyPointerMoveCalled() const
    {
        return m_notifyPointerMoveCalled;
    }

    const bool& GetNotifyTouchPushCalled() const
    {
        return m_notifyTouchPushCalled;
    }

    const bool& GetNotifyTouchReleaseCalled() const
    {
        return m_notifyTouchReleaseCalled;
    }

    const bool& GetNotifyTouchMoveCalled() const
    {
        return m_notifyTouchMoveCalled;
    }

    const bool& GetNotifyTouchEnterCalled() const
    {
        return m_notifyTouchEnterCalled;
    }

    void NotifyInput(InputType inputType,
                     InputAction inputAction,
                     Point point,
                     bool& updateScreen) override
    {
        Control::NotifyInput(inputType, inputAction, point, updateScreen);
        if (InputType::Pointer == inputType)
        {
            switch (inputAction)
            {
                case InputAction::EnterReleased:
                    break;
                case InputAction::EnterPushed:
                    break;
                case InputAction::Move:
                    m_notifyPointerMoveCalled = true;
                    break;
                case InputAction::Push:
                    m_notifyPointerPushCalled = true;
                    break;
                case InputAction::Release:
                    m_notifyPointerReleaseCalled = true;
                    break;
                case InputAction::Leave:
                    break;
            }
        }
        else // touch
        {
            switch (inputAction)
            {
                case InputAction::EnterReleased:
                    m_notifyTouchEnterCalled = true;
                    break;
                case InputAction::EnterPushed:
                    m_notifyTouchEnterCalled = true;
                    break;
                case InputAction::Move:
                    m_notifyTouchMoveCalled = true;
                    break;
                case InputAction::Push:
                    m_notifyTouchPushCalled = true;
                    break;
                case InputAction::Release:
                    m_notifyTouchReleaseCalled = true;
                    break;
                case InputAction::Leave:
                    break;
            }
        }
    }

    void ResetNotifications()
    {
        m_notifyPointerPushCalled    = false;
        m_notifyPointerMoveCalled    = false;
        m_notifyPointerReleaseCalled = false;

        m_notifyTouchEnterCalled   = false;
        m_notifyTouchReleaseCalled = false;
        m_notifyTouchPushCalled    = false;
        m_notifyTouchMoveCalled    = false;
    }

private:
    bool m_notifyPointerPushCalled    = false;
    bool m_notifyPointerMoveCalled    = false;
    bool m_notifyPointerReleaseCalled = false;

    bool m_notifyTouchPushCalled    = false;
    bool m_notifyTouchReleaseCalled = false;
    bool m_notifyTouchMoveCalled    = false;
    bool m_notifyTouchEnterCalled   = false;
};

TEST(ElementManagerTests, WhenControlIsCaptured_ItReceivesNotifyUp)
{
    auto em = make_shared<ElementManager>();
    auto sc = make_shared<StubControl>();
    em->SetRoot(sc);
    sc->SetLeft(1);
    sc->SetRight(2);
    sc->SetTop(1);
    sc->SetBottom(2);

    auto pointerInput = InputId(PointerInputId);
    em->NotifyNewPoint(pointerInput, Point{1, 1});
    em->NotifyDown(pointerInput);
    em->NotifyNewPoint(pointerInput, Point{0, 0});
    em->NotifyUp(pointerInput);

    ASSERT_EQ(true, sc->GetNotifyPointerReleaseCalled());
}

TEST(ElementManagerTests, WhenControlIsHidden_ItDoesNotReceiveNotifications)
{
    auto em = make_shared<ElementManager>();
    auto sc = make_shared<StubControl>();
    sc->SetIsVisible(false);

    em->SetRoot(sc);
    sc->SetLeft(1);
    sc->SetRight(2);
    sc->SetTop(1);
    sc->SetBottom(2);

    auto pointerInput = InputId(PointerInputId);
    em->NotifyNewPoint(pointerInput, Point{1, 1});
    em->NotifyDown(pointerInput);
    em->NotifyNewPoint(pointerInput, Point{1.5, 1.5});
    em->NotifyUp(pointerInput);

    ASSERT_EQ(false, sc->GetNotifyPointerPushCalled());
    ASSERT_EQ(false, sc->GetNotifyPointerReleaseCalled());
    ASSERT_EQ(false, sc->GetNotifyPointerMoveCalled());

    auto touchPoint = InputId(FirstTouchId);
    em->NotifyNewPoint(touchPoint, Point{1, 1});
    em->NotifyDown(touchPoint);
    em->NotifyNewPoint(touchPoint, Point{1.5, 1.5});
    em->NotifyUp(touchPoint);

    ASSERT_EQ(false, sc->GetNotifyTouchPushCalled());
    ASSERT_EQ(false, sc->GetNotifyTouchReleaseCalled());
    ASSERT_EQ(false, sc->GetNotifyTouchMoveCalled());
}

TEST(ElementManagerTests, WhenControlIsDisabled_ItDoesNotReceiveNotifications)
{
    auto em = make_shared<ElementManager>();
    auto sc = make_shared<StubControl>();
    sc->SetIsEnabled(false);

    em->SetRoot(sc);
    sc->SetLeft(1);
    sc->SetRight(2);
    sc->SetTop(1);
    sc->SetBottom(2);

    auto pointerInput = InputId(PointerInputId);
    em->NotifyNewPoint(pointerInput, Point{1, 1});
    em->NotifyDown(pointerInput);
    em->NotifyNewPoint(pointerInput, Point{1.5, 1.5});
    em->NotifyUp(pointerInput);

    ASSERT_EQ(false, sc->GetNotifyPointerPushCalled());
    ASSERT_EQ(false, sc->GetNotifyPointerReleaseCalled());
    ASSERT_EQ(false, sc->GetNotifyPointerMoveCalled());

    auto touchInput = InputId(FirstTouchId);
    em->NotifyNewPoint(touchInput, Point{1, 1});
    em->NotifyDown(touchInput);
    em->NotifyNewPoint(touchInput, Point{1.5, 1.5});
    em->NotifyUp(touchInput);

    ASSERT_EQ(false, sc->GetNotifyTouchPushCalled());
    ASSERT_EQ(false, sc->GetNotifyTouchReleaseCalled());
    ASSERT_EQ(false, sc->GetNotifyTouchMoveCalled());
}

TEST(ElementManagerTests, WhenControlIsTouchedDownAndUp_ItReceivesNotifications)
{
    auto em = make_shared<ElementManager>();
    auto sc = make_shared<StubControl>();
    em->SetRoot(sc);
    sc->SetLeft(1);
    sc->SetRight(2);
    sc->SetTop(1);
    sc->SetBottom(2);

    auto touchInput = InputId(FirstTouchId);
    em->NotifyNewPoint(touchInput, Point{1, 1});
    em->NotifyDown(touchInput);
    em->NotifyUp(touchInput);

    ASSERT_EQ(true, sc->GetNotifyTouchPushCalled());
    ASSERT_EQ(true, sc->GetNotifyTouchReleaseCalled());
}

TEST(ElementManagerTests, WhenControlIsTouchedDownLeavesAndReturns_ItReceivesNotifyEnter)
{
    auto em = make_shared<ElementManager>();
    auto sc = make_shared<StubControl>();
    em->SetRoot(sc);
    sc->SetLeft(1);
    sc->SetRight(2);
    sc->SetTop(1);
    sc->SetBottom(2);

    auto touchInput = InputId(FirstTouchId);
    em->NotifyNewPoint(touchInput, Point{1, 1});
    em->NotifyDown(touchInput);
    em->NotifyNewPoint(touchInput, Point{0, 0});
    em->NotifyNewPoint(touchInput, Point{1, 1});

    ASSERT_EQ(true, sc->GetNotifyTouchPushCalled());
    ASSERT_EQ(true, sc->GetNotifyTouchEnterCalled());
}

TEST(ElementManagerTests, WhenControlIsDownAndUpMultipleTimes_ItReceivesNotifications)
{
    auto em = make_shared<ElementManager>();
    auto sc = make_shared<StubControl>();
    em->SetRoot(sc);
    sc->SetLeft(1);
    sc->SetRight(2);
    sc->SetTop(1);
    sc->SetBottom(2);

    auto touchInput = InputId(PointerInputId);
    em->NotifyNewPoint(touchInput, Point{1, 1});
    em->NotifyDown(touchInput);
    em->NotifyUp(touchInput);

    ASSERT_EQ(true, sc->GetNotifyPointerPushCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerReleaseCalled());

    sc->ResetNotifications();

    em->NotifyDown(touchInput);
    em->NotifyUp(touchInput);

    ASSERT_EQ(true, sc->GetNotifyPointerPushCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerReleaseCalled());

}

