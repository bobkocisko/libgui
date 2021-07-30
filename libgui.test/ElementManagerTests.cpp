
#include "libgui/ElementManager.h"
#include "libgui/Location.h"
#include "libgui/Layer.h"

#include <algorithm>
#include <functional>
#include <vector>

#include <gtest/gtest.h>

using namespace libgui;

struct InputEntry
{
  InputType inputType;
  InputAction inputAction;
  Point point;
};


class StubControl: public Control
{
public:
  StubControl(Dependencies elementDependencies)
    : Control(elementDependencies)
  {
  }

  virtual ~StubControl()
  {
    if (_destructorCallback)
    {
      _destructorCallback();
    }
  }

  void SetDesctructorCallback(const std::function<void()>& callback)
  {
    _destructorCallback = callback;
  }

  bool GetNotifyPointerPushCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Pointer &&
      entry.inputAction == InputAction::Push;});
  }

  bool GetNotifyPointerReleaseCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Pointer &&
      entry.inputAction == InputAction::Release;});
  }

  bool GetNotifyPointerMoveCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Pointer &&
      entry.inputAction == InputAction::Move;});
  }

  bool GetNotifyPointerEscapeCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Pointer &&
      entry.inputAction == InputAction::EngagedEscape;});
  }

  bool GetNotifyPointerReturnCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Pointer &&
      entry.inputAction == InputAction::EngagedReturn;});
  }

  bool GetNotifyPointerLeaveCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Pointer &&
      entry.inputAction == InputAction::Leave;});
  }

  bool GetNotifyPointerEnterCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Pointer &&
      (entry.inputAction == InputAction::EnterReleased ||
       entry.inputAction == InputAction::EnterPushed);});
  }

  bool GetNotifyTouchPushCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Touch &&
      entry.inputAction == InputAction::Push;});
  }

  bool GetNotifyTouchReleaseCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Touch &&
      entry.inputAction == InputAction::Release;});
  }

  bool GetNotifyTouchMoveCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Touch &&
      entry.inputAction == InputAction::Move;});
  }

  bool GetNotifyTouchEnterPushedCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Touch &&
      entry.inputAction == InputAction::EnterPushed;});
  }

  bool GetNotifyTouchEnterReleasedCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Touch &&
      entry.inputAction == InputAction::EnterReleased;});
  }

  bool GetNotifyTouchLeaveCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Touch &&
      entry.inputAction == InputAction::Leave;});
  }

  bool GetNotifyTouchEscapeCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Touch &&
      entry.inputAction == InputAction::EngagedEscape;});
  }

  bool GetNotifyTouchReturnCalled() const
  {
    return AnyInputsMatch([](InputEntry entry) {
      return
      entry.inputType == InputType::Touch &&
      entry.inputAction == InputAction::EngagedReturn;});
  }

  void NotifyInput(InputType inputType,
                   InputAction inputAction,
                   Point point) override
  {
    Control::NotifyInput(inputType, inputAction, point);

    _capturedInputs.emplace_back(InputEntry{
      inputType,
      inputAction,
      point
    });
  }

  void ResetNotifications()
  {
    _capturedInputs.clear();
  }

private:
  std::function<void()> _destructorCallback;

  std::vector<InputEntry> _capturedInputs;

  bool AnyInputsMatch(const std::function<bool(InputEntry)>& criteria) const
  {
    return std::any_of(_capturedInputs.begin(), _capturedInputs.end(),
      criteria);
  }
};

TEST(ElementManagerTests, WhenControlIsCaptured_ItReceivesNotifyUp)
{
  auto em    = std::make_shared<ElementManager>();
  auto layer = em->CreateLayerAbove(nullptr);
  layer->SetLeft(0);
  layer->SetRight(10);
  layer->SetTop(0);
  layer->SetBottom(10);
  auto sc = layer->CreateChild<StubControl>();

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
  auto em    = std::make_shared<ElementManager>();
  auto layer = em->CreateLayerAbove(nullptr);
  layer->SetLeft(0);
  layer->SetRight(10);
  layer->SetTop(0);
  layer->SetBottom(10);
  auto sc = layer->CreateChild<StubControl>();

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
  auto em    = std::make_shared<ElementManager>();
  auto layer = em->CreateLayerAbove(nullptr);
  layer->SetLeft(0);
  layer->SetRight(10);
  layer->SetTop(0);
  layer->SetBottom(10);
  auto sc = layer->CreateChild<StubControl>();

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
  auto em    = std::make_shared<ElementManager>();
  auto layer = em->CreateLayerAbove(nullptr);
  layer->SetLeft(0);
  layer->SetRight(10);
  layer->SetTop(0);
  layer->SetBottom(10);
  auto sc = layer->CreateChild<StubControl>();

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
  auto em   = std::make_shared<ElementManager>();
  auto root = em->CreateLayerAbove(nullptr);
  root->SetLeft(0);
  root->SetRight(10);
  root->SetTop(0);
  root->SetBottom(10);

  auto sc = root->CreateChild<StubControl>();

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
  auto em    = std::make_shared<ElementManager>();
  auto layer = em->CreateLayerAbove(nullptr);
  auto sc    = layer->CreateChild<StubControl>();
  layer->SetLeft(0);
  layer->SetRight(10);
  layer->SetTop(0);
  layer->SetBottom(10);

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
  auto em    = std::make_shared<ElementManager>();
  auto layer = em->CreateLayerAbove(nullptr);
  auto sc    = layer->CreateChild<StubControl>();
  layer->SetLeft(0);
  layer->SetRight(1000);
  layer->SetTop(0);
  layer->SetBottom(1000);

  sc->SetLeft(100);
  sc->SetRight(200);
  sc->SetTop(100);
  sc->SetBottom(200);

  auto touchInput = InputId(FirstTouchId);
  em->NotifyNewPoint(touchInput, Point{100, 100});
  em->NotifyDown(touchInput);
  em->NotifyNewPoint(touchInput, Point{0, 0});
  em->NotifyNewPoint(touchInput, Point{100, 100});

  ASSERT_EQ(true, sc->GetNotifyTouchPushCalled());
  ASSERT_EQ(true, sc->GetNotifyTouchReturnCalled());
}

TEST(ElementManagerTests, WhenControlIsDownAndUpMultipleTimes_ItReceivesNotifications)
{
  auto em    = std::make_shared<ElementManager>();
  auto layer = em->CreateLayerAbove(nullptr);
  auto sc    = layer->CreateChild<StubControl>();
  layer->SetLeft(0);
  layer->SetRight(10);
  layer->SetTop(0);
  layer->SetBottom(10);

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
  auto em    = std::make_shared<ElementManager>();
  auto layer = em->CreateLayerAbove(nullptr);
  auto sc    = layer->CreateChild<StubControl>();
  layer->SetLeft(0);
  layer->SetRight(10);
  layer->SetTop(0);
  layer->SetBottom(10);

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
  auto em    = std::make_shared<ElementManager>();
  auto layer = em->CreateLayerAbove(nullptr);
  auto sc    = layer->CreateChild<StubControl>();
  layer->SetLeft(0);
  layer->SetRight(1000);
  layer->SetTop(0);
  layer->SetBottom(1000);

  sc->SetLeft(100);
  sc->SetRight(200);
  sc->SetTop(100);
  sc->SetBottom(200);

  auto touchInput = InputId(FirstTouchId);
  em->NotifyNewPoint(touchInput, Point{100, 100});
  em->NotifyDown(touchInput);
  ASSERT_EQ(true, sc->GetNotifyTouchEnterPushedCalled());
  ASSERT_EQ(true, sc->GetNotifyTouchPushCalled());

  em->NotifyNewPoint(touchInput, Point{100.1, 100.1});
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
  auto em    = std::make_shared<ElementManager>();
  auto layer = em->CreateLayerAbove(nullptr);
  auto sc    = layer->CreateChild<StubControl>();
  layer->SetLeft(0);
  layer->SetRight(1000);
  layer->SetTop(0);
  layer->SetBottom(1000);

  sc->SetLeft(100);
  sc->SetRight(200);
  sc->SetTop(100);
  sc->SetBottom(200);

  auto touchInput = InputId(FirstTouchId);
  em->NotifyNewPoint(touchInput, Point{100, 100});
  em->NotifyDown(touchInput);
  ASSERT_EQ(true, sc->GetNotifyTouchEnterPushedCalled());
  ASSERT_EQ(true, sc->GetNotifyTouchPushCalled());

  em->NotifyNewPoint(touchInput, Point{100.1, 100.1});
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
  auto em    = std::make_shared<ElementManager>();
  auto layer = em->CreateLayerAbove(nullptr);
  layer->SetLeft(0);
  layer->SetRight(10);
  layer->SetTop(0);
  layer->SetBottom(10);
  auto sc = layer->CreateChild<StubControl>();

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

  layer->RemoveChildren(Element::UpdateWhenRemoving::No);
  sc = nullptr;

  ASSERT_EQ(true, isDestroyed);

  // If any attempt is made to send notifications to the control, this test will SEGFAULT
  em->NotifyNewPoint(pointerInput, Point{1.5, 1.5});
}

