#include <gtest/gtest.h>
#include "include/Common.h"
#include "libgui/ElementManager.h"
#include "libgui/Location.h"
#include "libgui/Layer.h"

using namespace std;
using namespace libgui;

class StubControl: public Control
{
public:
    virtual ~StubControl()
    {
        if (_destructorCallback)
        {
            _destructorCallback();
        }
    }

    void SetDesctructorCallback(const function<void()>& callback)
    {
        _destructorCallback = callback;
    }

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

    bool GetNotifyPointerEnterCalled() const
    {
        return _notifyPointerEnterCalled;
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

    bool GetNotifyTouchEnterPushedCalled() const
    {
        return _notifyTouchEnterPushedCalled;
    }

    bool GetNotifyTouchEnterReleasedCalled() const
    {
        return _notifyTouchEnterReleasedCalled;
    }

    bool GetNotifyTouchLeaveCalled() const
    {
        return _notifyTouchLeaveCalled;
    }

    bool GetNotifyTouchEscapeCalled() const
    {
        return _notifyTouchEscapeCalled;
    }

    bool GetNotifyTouchReturnCalled() const
    {
        return _notifyTouchReturnCalled;
    }

    void NotifyInput(InputType inputType,
                     InputAction inputAction,
                     Point point) override
    {
        Control::NotifyInput(inputType, inputAction, point);
        if (InputType::Pointer == inputType)
        {
            switch (inputAction)
            {
                case InputAction::EnterReleased:
                    _notifyPointerEnterCalled = true;
                    break;
                case InputAction::EnterPushed:
                    _notifyPointerEnterCalled = true;
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
                    _notifyTouchEnterReleasedCalled = true;
                    break;
                case InputAction::EnterPushed:
                    _notifyTouchEnterPushedCalled = true;
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
                    _notifyTouchLeaveCalled = true;
                    break;
                case InputAction::EngagedEscape:
                    _notifyTouchEscapeCalled = true;
                    break;
                case InputAction::EngagedReturn:
                    _notifyTouchReturnCalled = true;
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
        _notifyPointerEnterCalled   = false;

        _notifyTouchEnterPushedCalled   = false;
        _notifyTouchEnterReleasedCalled = false;
        _notifyTouchReleaseCalled       = false;
        _notifyTouchPushCalled          = false;
        _notifyTouchMoveCalled          = false;
        _notifyTouchLeaveCalled         = false;
        _notifyTouchEscapeCalled        = false;
        _notifyTouchReturnCalled        = false;
    }

private:
    function<void()> _destructorCallback;

    bool _notifyPointerPushCalled    = false;
    bool _notifyPointerMoveCalled    = false;
    bool _notifyPointerReleaseCalled = false;
    bool _notifyPointerEscapeCalled  = false;
    bool _notifyPointerReturnCalled  = false;
    bool _notifyPointerEnterCalled   = false;

    bool _notifyPointerLeaveCalled       = false;
    bool _notifyTouchPushCalled          = false;
    bool _notifyTouchReleaseCalled       = false;
    bool _notifyTouchMoveCalled          = false;
    bool _notifyTouchEnterPushedCalled   = false;
    bool _notifyTouchEnterReleasedCalled = false;
    bool _notifyTouchLeaveCalled         = false;
    bool _notifyTouchEscapeCalled        = false;
    bool _notifyTouchReturnCalled        = false;
};

TEST(ElementManagerTests, WhenControlIsCaptured_ItReceivesNotifyUp)
{
    auto em    = make_shared<ElementManager>();
    auto layer = em->AddLayerAbove(nullptr);
    layer->SetLeft(0);
    layer->SetRight(10);
    layer->SetTop(0);
    layer->SetBottom(10);
    auto sc = make_shared<StubControl>();
    layer->AddChild(sc);
    layer->InitializeAll();

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
    auto em    = make_shared<ElementManager>();
    auto layer = em->AddLayerAbove(nullptr);
    layer->SetLeft(0);
    layer->SetRight(10);
    layer->SetTop(0);
    layer->SetBottom(10);
    auto sc = make_shared<StubControl>();
    layer->AddChild(sc);
    layer->InitializeAll();

    sc->SetIsVisible(false);

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
    auto em    = make_shared<ElementManager>();
    auto layer = em->AddLayerAbove(nullptr);
    layer->SetLeft(0);
    layer->SetRight(10);
    layer->SetTop(0);
    layer->SetBottom(10);
    auto sc = make_shared<StubControl>();
    layer->AddChild(sc);
    layer->InitializeAll();

    sc->SetIsEnabled(false);

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

TEST(ElementManagerTests, WhenActiveControlBecomesDisabled_ItReceivesLeaveNotification)
{
    auto em    = make_shared<ElementManager>();
    auto layer = em->AddLayerAbove(nullptr);
    layer->SetLeft(0);
    layer->SetRight(10);
    layer->SetTop(0);
    layer->SetBottom(10);
    auto sc = make_shared<StubControl>();
    layer->AddChild(sc);
    layer->InitializeAll();

    sc->SetLeft(1);
    sc->SetRight(2);
    sc->SetTop(1);
    sc->SetBottom(2);

    auto pointerInput = InputId(PointerInputId);
    em->NotifyNewPoint(pointerInput, Point{1, 1});
    em->NotifyDown(pointerInput);
    em->NotifyNewPoint(pointerInput, Point{1.5, 1.5});
    em->NotifyUp(pointerInput);

    ASSERT_EQ(true, sc->GetNotifyPointerPushCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerReleaseCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerMoveCalled());

    sc->ResetNotifications();

    // The control is still 'active' at this point because the pointer has not left its bounds yet

    sc->SetIsEnabled(false);

    em->NotifyDown(pointerInput);

    ASSERT_EQ(false, sc->GetNotifyPointerPushCalled());
    ASSERT_EQ(false, sc->GetNotifyPointerReleaseCalled());
    ASSERT_EQ(false, sc->GetNotifyPointerMoveCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerLeaveCalled());

}

TEST(ElementManagerTests, WhenActiveControlParentBecomesDisabled_ItReceivesLeaveOnMove)
{
    auto em   = make_shared<ElementManager>();
    auto root = em->AddLayerAbove(nullptr);
    root->SetLeft(0);
    root->SetRight(10);
    root->SetTop(0);
    root->SetBottom(10);

    auto sc = make_shared<StubControl>();
    root->AddChild(sc);
    root->InitializeAll();

    sc->SetLeft(1);
    sc->SetRight(2);
    sc->SetTop(1);
    sc->SetBottom(2);

    auto pointerInput = InputId(PointerInputId);
    em->NotifyNewPoint(pointerInput, Point{1, 1});
    em->NotifyDown(pointerInput);
    em->NotifyNewPoint(pointerInput, Point{1.5, 1.5});
    em->NotifyUp(pointerInput);

    ASSERT_EQ(true, sc->GetNotifyPointerPushCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerReleaseCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerMoveCalled());

    sc->ResetNotifications();

    // The control is still 'active' at this point because the pointer has not left its bounds yet

    root->SetIsEnabled(false);

    em->NotifyNewPoint(pointerInput, Point{1.7, 1.7});

    ASSERT_EQ(false, sc->GetNotifyPointerPushCalled());
    ASSERT_EQ(false, sc->GetNotifyPointerReleaseCalled());
    ASSERT_EQ(false, sc->GetNotifyPointerMoveCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerLeaveCalled());

    // Now enable the control and move the pointer

    root->SetIsEnabled(true);

    em->NotifyNewPoint(pointerInput, Point{1.5, 1.5});

    ASSERT_EQ(true, sc->GetNotifyPointerEnterCalled());
    ASSERT_EQ(true, sc->GetNotifyPointerMoveCalled());

}

TEST(ElementManagerTests, WhenControlIsTouchedDownAndUp_ItReceivesNotifications)
{
    auto em    = make_shared<ElementManager>();
    auto layer = em->AddLayerAbove(nullptr);
    auto sc    = make_shared<StubControl>();
    layer->AddChild(sc);
    layer->SetLeft(0);
    layer->SetRight(10);
    layer->SetTop(0);
    layer->SetBottom(10);
    layer->InitializeAll();

    sc->SetLeft(1);
    sc->SetRight(2);
    sc->SetTop(1);
    sc->SetBottom(2);

    auto touchInput = InputId(FirstTouchId);
    em->NotifyNewPoint(touchInput, Point{1, 1});
    em->NotifyDown(touchInput);
    ASSERT_EQ(true, sc->GetNotifyTouchEnterPushedCalled());
    ASSERT_EQ(true, sc->GetNotifyTouchPushCalled());

    em->NotifyUp(touchInput);
    ASSERT_EQ(true, sc->GetNotifyTouchReleaseCalled());
    ASSERT_EQ(true, sc->GetNotifyTouchLeaveCalled());
}

TEST(ElementManagerTests, WhenControlIsTouchedDownLeavesAndReturns_ItReceivesEscapedReturn)
{
    auto em    = make_shared<ElementManager>();
    auto layer = em->AddLayerAbove(nullptr);
    auto sc    = make_shared<StubControl>();
    layer->AddChild(sc);
    layer->SetLeft(0);
    layer->SetRight(10);
    layer->SetTop(0);
    layer->SetBottom(10);
    layer->InitializeAll();

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
    ASSERT_EQ(true, sc->GetNotifyTouchReturnCalled());
}

TEST(ElementManagerTests, WhenControlIsDownAndUpMultipleTimes_ItReceivesNotifications)
{
    auto em    = make_shared<ElementManager>();
    auto layer = em->AddLayerAbove(nullptr);
    auto sc    = make_shared<StubControl>();
    layer->AddChild(sc);
    layer->SetLeft(0);
    layer->SetRight(10);
    layer->SetTop(0);
    layer->SetBottom(10);
    layer->InitializeAll();

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
    auto em    = make_shared<ElementManager>();
    auto layer = em->AddLayerAbove(nullptr);
    auto sc    = make_shared<StubControl>();
    layer->AddChild(sc);
    layer->SetLeft(0);
    layer->SetRight(10);
    layer->SetTop(0);
    layer->SetBottom(10);
    layer->InitializeAll();

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

TEST(ElementManagerTests, WhenControlIsTouchDragged_ItReceivesNotifications)
{
    auto em    = make_shared<ElementManager>();
    auto layer = em->AddLayerAbove(nullptr);
    auto sc    = make_shared<StubControl>();
    layer->AddChild(sc);
    layer->SetLeft(0);
    layer->SetRight(10);
    layer->SetTop(0);
    layer->SetBottom(10);
    layer->InitializeAll();

    sc->SetLeft(1);
    sc->SetRight(2);
    sc->SetTop(1);
    sc->SetBottom(2);

    auto touchInput = InputId(FirstTouchId);
    em->NotifyNewPoint(touchInput, Point{1, 1});
    em->NotifyDown(touchInput);
    ASSERT_EQ(true, sc->GetNotifyTouchEnterPushedCalled());
    ASSERT_EQ(true, sc->GetNotifyTouchPushCalled());

    em->NotifyNewPoint(touchInput, Point{1.1, 1.1});
    ASSERT_EQ(true, sc->GetNotifyTouchMoveCalled());

    em->NotifyNewPoint(touchInput, Point{0.1, 0.1});
    ASSERT_EQ(true, sc->GetNotifyTouchEscapeCalled());

    em->NotifyNewPoint(touchInput, Point{0, 0});
    ASSERT_EQ(true, sc->GetNotifyTouchMoveCalled());

    em->NotifyUp(touchInput);
    ASSERT_EQ(true, sc->GetNotifyTouchReleaseCalled());
    ASSERT_EQ(true, sc->GetNotifyTouchLeaveCalled());

}

TEST(ElementManagerTests, WhenControlIsTouchEngagedRemotelyThenDisabled_ItStopsReceivingNotifications)
{
    auto em    = make_shared<ElementManager>();
    auto layer = em->AddLayerAbove(nullptr);
    auto sc    = make_shared<StubControl>();
    layer->AddChild(sc);
    layer->SetLeft(0);
    layer->SetRight(10);
    layer->SetTop(0);
    layer->SetBottom(10);
    layer->InitializeAll();

    sc->SetLeft(1);
    sc->SetRight(2);
    sc->SetTop(1);
    sc->SetBottom(2);

    auto touchInput = InputId(FirstTouchId);
    em->NotifyNewPoint(touchInput, Point{1, 1});
    em->NotifyDown(touchInput);
    ASSERT_EQ(true, sc->GetNotifyTouchEnterPushedCalled());
    ASSERT_EQ(true, sc->GetNotifyTouchPushCalled());

    em->NotifyNewPoint(touchInput, Point{1.1, 1.1});
    ASSERT_EQ(true, sc->GetNotifyTouchMoveCalled());

    em->NotifyNewPoint(touchInput, Point{0.1, 0.1});
    ASSERT_EQ(true, sc->GetNotifyTouchEscapeCalled());

    em->NotifyNewPoint(touchInput, Point{0, 0});
    ASSERT_EQ(true, sc->GetNotifyTouchMoveCalled());

    sc->SetIsEnabled(false);

    sc->ResetNotifications();

    em->NotifyNewPoint(touchInput, Point{0.1, 0.1});
    ASSERT_EQ(false, sc->GetNotifyTouchMoveCalled());
    ASSERT_EQ(true, sc->GetNotifyTouchLeaveCalled());

}

TEST(ElementManagerTests, AfterControlIsDestroyed_ItIsNotSentNotifications)
{
    auto em    = make_shared<ElementManager>();
    auto layer = em->AddLayerAbove(nullptr);
    layer->SetLeft(0);
    layer->SetRight(10);
    layer->SetTop(0);
    layer->SetBottom(10);
    auto sc = make_shared<StubControl>();
    layer->AddChild(sc);
    layer->InitializeAll();

    sc->SetLeft(1);
    sc->SetRight(2);
    sc->SetTop(1);
    sc->SetBottom(2);

    auto pointerInput = InputId(PointerInputId);

    em->NotifyNewPoint(pointerInput, Point{1.5, 1.5});
    ASSERT_EQ(true, sc->GetNotifyPointerEnterCalled());

    em->NotifyDown(pointerInput);
    ASSERT_EQ(true, sc->GetNotifyPointerPushCalled());

    em->NotifyUp(pointerInput);
    ASSERT_EQ(true, sc->GetNotifyPointerReleaseCalled());

    bool isDestroyed = false;
    sc->SetDesctructorCallback([&isDestroyed]() { isDestroyed = true; });

    layer->RemoveChildren();
    sc = nullptr;

    ASSERT_EQ(true, isDestroyed);

    // If any attempt is made to send notifications to the control, this test will SEGFAULT
    em->NotifyNewPoint(pointerInput, Point{1.5, 1.5});
}

