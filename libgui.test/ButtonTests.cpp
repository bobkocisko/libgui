#include "include/Common.h"
#include "libgui/Button.h"
#include "libgui/ElementManager.h"
#include "libgui/Layer.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace libgui;
using namespace std;

using ::testing::Return;


TEST(ButtonTests, WhenPointerEntering_StateIsPending)
{
    auto btn = make_shared<Button>();
    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);

    ASSERT_EQ(Button::VisibleState::Pending, btn->GetVisibleState());
}

TEST(ButtonTests, WhenPointerLeaving_StateIsIdle)
{
    auto btn = make_shared<Button>();
    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Leave, Point(), updateScreen);
    ASSERT_EQ(Button::VisibleState::Idle, btn->GetVisibleState());
}

TEST(ButtonTests, WhenPointerPushed_StateIsEngaged)
{
    auto em   = make_shared<ElementManager>();
    auto root = em->AddLayer();
    auto btn  = make_shared<Button>();
    root->AddChild(btn);
    root->InitializeAll();

    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);
    ASSERT_EQ(Button::VisibleState::Engaged, btn->GetVisibleState());
}

TEST(ButtonTests, WhenPointerReleased_StateIsPending)
{
    auto btn = make_shared<Button>();
    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterPushed, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Release, Point(), updateScreen);
    ASSERT_EQ(Button::VisibleState::Pending, btn->GetVisibleState());
}

TEST(ButtonTests, WhenPointerPushedAndEscaping_StateIsEngagedRemotely)
{
    auto em   = make_shared<ElementManager>();
    auto root = em->AddLayer();
    auto btn  = make_shared<Button>();
    root->AddChild(btn);
    root->InitializeAll();

    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::EngagedEscape, Point(), updateScreen);
    ASSERT_EQ(Button::VisibleState::EngagedRemotely, btn->GetVisibleState());
}

TEST(ButtonTests, WhenPointerPushedAndReturning_StateIsEngaged)
{
    auto em   = make_shared<ElementManager>();
    auto root = em->AddLayer();
    auto btn  = make_shared<Button>();
    root->AddChild(btn);
    root->InitializeAll();

    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::EngagedEscape, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::EngagedReturn, Point(), updateScreen);
    ASSERT_EQ(Button::VisibleState::Engaged, btn->GetVisibleState());
}

TEST(ButtonTests, WhenPointerEscapedAndReleasedOutside_StateIsIdle)
{
    auto em   = make_shared<ElementManager>();
    auto root = em->AddLayer();
    auto btn  = make_shared<Button>();
    root->AddChild(btn);
    root->InitializeAll();

    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::EngagedEscape, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Release, Point(), updateScreen);
    ASSERT_EQ(Button::VisibleState::Idle, btn->GetVisibleState());
}

TEST(ButtonTests, WhenPointerEscapedAndReleasedOutside_NoClick)
{
    auto em   = make_shared<ElementManager>();
    auto root = em->AddLayer();
    auto btn  = make_shared<Button>();
    root->AddChild(btn);
    root->InitializeAll();

    bool is_clicked = false;
    btn->SetEventCallback([&](shared_ptr<Button> b, Button::OutputEvent event)
                          {
                              if (Button::OutputEvent::Clicked == event)
                              {
                                  is_clicked = true;
                              }
                          });

    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::EngagedEscape, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Release, Point(), updateScreen);
    ASSERT_EQ(false, is_clicked);
}

TEST(ButtonTests, AfterPointerClicked_StateIsPending)
{
    auto em   = make_shared<ElementManager>();
    auto root = em->AddLayer();
    auto btn  = make_shared<Button>();
    root->AddChild(btn);
    root->InitializeAll();

    bool updateScreen;
    btn->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Pointer, InputAction::Release, Point(), updateScreen);
    ASSERT_EQ(Button::VisibleState::Pending, btn->GetVisibleState());
}

TEST(ButtonTests, WhenTouchPushAndRelease_Click)
{
    auto em   = make_shared<ElementManager>();
    auto root = em->AddLayer();
    auto btn  = make_shared<Button>();
    root->AddChild(btn);
    root->InitializeAll();

    bool isClicked = false;
    btn->SetEventCallback([&](shared_ptr<Button> b, Button::OutputEvent event)
                          {
                              if (Button::OutputEvent::Clicked == event)
                              {
                                  isClicked = true;
                              }
                          });

    bool updateScreen;
    btn->NotifyInput(InputType::Touch, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Touch, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Touch, InputAction::Release, Point(), updateScreen);
    ASSERT_EQ(true, isClicked);
}

TEST(ButtonTests, WhenTouchDownEscapeReturnAndUp_StateIsPending)
{
    auto em   = make_shared<ElementManager>();
    auto root = em->AddLayer();
    auto btn  = make_shared<Button>();
    root->AddChild(btn);
    root->InitializeAll();

    bool updateScreen;
    btn->NotifyInput(InputType::Touch, InputAction::EnterReleased, Point(), updateScreen);
    btn->NotifyInput(InputType::Touch, InputAction::Push, Point(), updateScreen);
    btn->NotifyInput(InputType::Touch, InputAction::EngagedEscape, Point(), updateScreen);
    btn->NotifyInput(InputType::Touch, InputAction::EngagedReturn, Point(), updateScreen);
    btn->NotifyInput(InputType::Touch, InputAction::Release, Point(), updateScreen);

    ASSERT_EQ(Button::VisibleState::Pending, btn->GetVisibleState());
}

TEST(ButtonTests, WhenMultiplePushAndRelease_MultipleOutputEvents)
{
    auto em   = make_shared<ElementManager>();
    auto root = em->AddLayer();
    auto btn  = make_shared<Button>();
    root->AddChild(btn);
    root->InitializeAll();

    bool is_pushed = false;
    btn->SetEventCallback([&](shared_ptr<Button> b, Button::OutputEvent event)
                          {
                              if (Button::OutputEvent::Pushed == event)
                              {
                                  is_pushed = true;
                              }
                          });

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
