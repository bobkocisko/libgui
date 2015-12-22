#include "include/Common.h"
#include "libgui/Button.h"
#include "libgui/ElementManager.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
using namespace libgui;
using namespace std;

using ::testing::Return;


TEST(ButtonTests, WhenPointerEntering_StateIsHot)
{
    auto btn = make_shared<Button>();
    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);

    ASSERT_EQ(Button::VisibleState::Hot_Up, btn->GetVisibleState());
}

TEST(ButtonTests, WhenPointerLeaving_StateIsColdUp)
{
    auto btn = make_shared<Button>();
    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Leave, Point(), updateScreen);
    ASSERT_EQ(Button::VisibleState::Cold_Up, btn->GetVisibleState());
}

TEST(ButtonTests, WhenPointerPushed_StateIsHotAndDown)
{
    auto em  = make_shared<ElementManager>();
    auto btn = make_shared<Button>();
    em->SetRoot(btn);
    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);
    ASSERT_EQ(Button::VisibleState::Hot_Down, btn->GetVisibleState());
}

TEST(ButtonTests, WhenPointerReleased_StateIsHotAndUp)
{
    auto btn = make_shared<Button>();
    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterPushed, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Release, Point(), updateScreen);
    ASSERT_EQ(Button::VisibleState::Hot_Up, btn->GetVisibleState());
}

TEST(ButtonTests, WhenPointerPushedAndLeaving_StateIsColdClaimed)
{
    auto em  = make_shared<ElementManager>();
    auto btn = make_shared<Button>();
    em->SetRoot(btn);
    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Leave, Point(), updateScreen);
    ASSERT_EQ(Button::VisibleState::Cold_Claimed, btn->GetVisibleState());
}

TEST(ButtonTests, WhenPointerPushedAndReturning_StateIsHotAndDown)
{
    auto em  = make_shared<ElementManager>();
    auto btn = make_shared<Button>();
    em->SetRoot(btn);
    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Leave, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::EnterPushed, Point(), updateScreen);
    ASSERT_EQ(Button::VisibleState::Hot_Down, btn->GetVisibleState());
}

TEST(ButtonTests, WhenPointerLeftAndReleasedOutside_StateIsColdAndUp)
{
    auto em  = make_shared<ElementManager>();
    auto btn = make_shared<Button>();
    em->SetRoot(btn);
    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Leave, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Release, Point(), updateScreen);
    ASSERT_EQ(Button::VisibleState::Cold_Up, btn->GetVisibleState());
}

TEST(ButtonTests, WhenPointerLeftAndReleasedOutside_NoClick)
{
    auto em         = make_shared<ElementManager>();
    auto btn        = make_shared<Button>();
    bool is_clicked = false;
    btn->SetEventCallback([&](shared_ptr<Button> b, Button::OutputEvent event)
                          {
                              if (Button::OutputEvent::Clicked == event)
                              {
                                  is_clicked = true;
                              }
                          });

    em->SetRoot(btn);
    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Leave, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Release, Point(), updateScreen);
    ASSERT_EQ(false, is_clicked);
}

TEST(ButtonTests, AfterPointerClicked_StateIsHotAndUp)
{
    auto em  = make_shared<ElementManager>();
    auto btn = make_shared<Button>();
    em->SetRoot(btn);
    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Release, Point(), updateScreen);
    ASSERT_EQ(Button::VisibleState::Hot_Up, btn->GetVisibleState());
}

TEST(ButtonTests, WhenTouchPushAndRelease_Click)
{
    auto em        = make_shared<ElementManager>();
    auto btn       = make_shared<Button>();
    bool isClicked = false;
    btn->SetEventCallback([&](shared_ptr<Button> b, Button::OutputEvent event)
                          {
                              if (Button::OutputEvent::Clicked == event)
                              {
                                  isClicked = true;
                              }
                          });

    em->SetRoot(btn);
    bool updateScreen;
    btn->NotifyInput(InputType::Touch, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Touch, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Touch, InputAction::Release, Point(), updateScreen);
    ASSERT_EQ(true, isClicked);
}

TEST(ButtonTests, WhenTouchDownLeaveReturnAndUp_StateIsHotAndUp)
{
    auto em  = make_shared<ElementManager>();
    auto btn = make_shared<Button>();

    em->SetRoot(btn);
    bool updateScreen;
    btn->NotifyInput(InputType::Touch, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Touch, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Touch, InputAction::Leave, Point(), updateScreen);
    btn->NotifyInput(InputType::Touch, InputAction::EnterPushed, Point(), updateScreen);
    btn->NotifyInput(InputType::Touch, InputAction::Release, Point(), updateScreen);

    ASSERT_EQ(Button::VisibleState::Hot_Up, btn->GetVisibleState());
}

TEST(ButtonTests, WhenMultiplePushAndRelease_MultipleOutputEvents)
{
    auto em        = make_shared<ElementManager>();
    auto btn       = make_shared<Button>();
    bool is_pushed = false;
    btn->SetEventCallback([&](shared_ptr<Button> b, Button::OutputEvent event)
                          {
                              if (Button::OutputEvent::Pushed == event)
                              {
                                  is_pushed = true;
                              }
                          });

    em->SetRoot(btn);
    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Release, Point(), updateScreen);
    ASSERT_EQ(true, is_pushed);

    // Now do it again
    is_pushed = false;
    btn->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Release, Point(), updateScreen);
    ASSERT_EQ(true, is_pushed);
}
