#include <gtest/gtest.h>
#include "include/Common.h"
#include "libgui/ElementManager.h"
#include "libgui/Location.h"

using namespace std;
using namespace libgui;

class StubControl: public Control
{
public:
    bool GetNotifyPointerPushCalled() const
    {
        return _notifyPointerPushCalled;
    }

    bool GetNotifyPointerReleaseCalled() const
    {
        return _notifyPointerReleaseCalled;
    }

    bool GetNotifyPointerMoveCalled() const
    {
        return _notifyPointerMoveCalled;
    }

    bool GetNotifyPointerEscapeCalled() const
    {
        return _notifyPointerEscapeCalled;
    }

    bool GetNotifyPointerReturnCalled() const
    {
        return _notifyPointerReturnCalled;
    }

    bool GetNotifyPointerLeaveCalled() const
    {
        return _notifyPointerLeaveCalled;
    }

    bool GetNotifyTouchPushCalled() const
    {
        return _notifyTouchPushCalled;
    }

    bool GetNotifyTouchReleaseCalled() const
    {
        return _notifyTouchReleaseCalled;
    }

    bool GetNotifyTouchMoveCalled() const
    {
        return _notifyTouchMoveCalled;
    }

    bool GetNotifyTouchEnterCalled() const
    {
        return _notifyTouchEnterCalled;
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
                    _notifyPointerMoveCalled = true;
                    break;
                case InputAction::Push:
                    _notifyPointerPushCalled = true;
                    break;
                case InputAction::Release:
                    _notifyPointerReleaseCalled = true;
                    break;
                case InputAction::Leave:
                    _notifyPointerLeaveCalled = true;
                    break;
                case InputAction::EngagedEscape:
                    _notifyPointerEscapeCalled = true;
                    break;
                case InputAction::EngagedReturn:
                    _notifyPointerReturnCalled = true;
                    break;
            }
        }
        else // touch
        {
            switch (inputAction)
            {
                case InputAction::EnterReleased:
                    _notifyTouchEnterCalled = true;
                    break;
                case InputAction::EnterPushed:
                    _notifyTouchEnterCalled = true;
                    break;
                case InputAction::Move:
                    _notifyTouchMoveCalled = true;
                    break;
                case InputAction::Push:
                    _notifyTouchPushCalled = true;
                    break;
                case InputAction::Release:
                    _notifyTouchReleaseCalled = true;
                    break;
                case InputAction::Leave:
                    break;
                case InputAction::EngagedEscape:
                    break;
                case InputAction::EngagedReturn:
                    break;
            }
        }
    }

    void ResetNotifications()
    {
        _notifyPointerPushCalled    = false;
        _notifyPointerMoveCalled    = false;
        _notifyPointerReleaseCalled = false;
        _notifyPointerEscapeCalled  = false;
        _notifyPointerReturnCalled  = false;
        _notifyPointerLeaveCalled   = false;

        _notifyTouchEnterCalled   = false;
        _notifyTouchReleaseCalled = false;
        _notifyTouchPushCalled    = false;
        _notifyTouchMoveCalled    = false;
    }

private:
    bool _notifyPointerPushCalled    = false;
    bool _notifyPointerMoveCalled    = false;
    bool _notifyPointerReleaseCalled = false;
    bool _notifyPointerEscapeCalled  = false;
    bool _notifyPointerReturnCalled  = false;
    bool _notifyPointerLeaveCalled   = false;

    bool _notifyTouchPushCalled    = false;
    bool _notifyTouchReleaseCalled = false;
    bool _notifyTouchMoveCalled    = false;
    bool _notifyTouchEnterCalled   = false;
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

    auto pointerInput = InputId(PointerInputId);
    em->NotifyNewPoint(pointerInput, Point{1, 1});
    em->NotifyDown(pointerInput);
    em->NotifyUp(pointerInput);

    ASSERT_EQ(true, sc->GetNotifyPointerPushCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerReleaseCalled());

    sc->ResetNotifications();

    em->NotifyDown(pointerInput);
    em->NotifyUp(pointerInput);

    ASSERT_EQ(true, sc->GetNotifyPointerPushCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerReleaseCalled());

}

TEST(ElementManagerTests, WhenDownEscapeRelease_NotificationsAreSent)
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

    ASSERT_EQ(true, sc->GetNotifyPointerPushCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerEscapeCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerEscapeCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerReleaseCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerLeaveCalled());

}
