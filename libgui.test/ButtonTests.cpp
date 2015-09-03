#include "include/Common.h"
#include "Button.h"
#include "ElementManager.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
using namespace libgui;
using namespace std;

using ::testing::Return;


TEST(ButtonTests, WhenMouseEntering_StateIsHot)
{
    auto btn = make_shared<Button>();
    btn->NotifyMouseEnter();

    ASSERT_EQ(true, btn->IsHot());
}

TEST(ButtonTests, WhenMouseLeaving_StateIsNotHot)
{
    auto btn = make_shared<Button>();
    btn->NotifyMouseEnter();
    btn->NotifyMouseLeave();
    ASSERT_EQ(false, btn->IsHot());
}

TEST(ButtonTests, WhenMouseDowning_StateIsHotAndPressed)
{
    auto em = make_shared<ElementManager>();
    auto btn = make_shared<Button>();
    em->SetRoot(btn);
    btn->NotifyMouseDown(Location());
    ASSERT_EQ(true, btn->IsHot());
    ASSERT_EQ(true, btn->IsPressed());
}

TEST(ButtonTests, WhenMouseUpping_StateIsNotPressed)
{
    auto btn = make_shared<Button>();
    btn->NotifyMouseUp(Location());
    ASSERT_EQ(false, btn->IsPressed());
}

TEST(ButtonTests, WhenMouseDownAndLeaving_StateIsHotAndNotPressed)
{
    auto em = make_shared<ElementManager>();
    auto btn = make_shared<Button>();
    em->SetRoot(btn);
    btn->NotifyMouseDown(Location());
    btn->NotifyMouseLeave();
    ASSERT_EQ(true, btn->IsHot());
    ASSERT_EQ(false, btn->IsPressed());
}

TEST(ButtonTests, WhenMouseDownAndReturning_StateIsHotAndPressed)
{
    auto em = make_shared<ElementManager>();
    auto btn = make_shared<Button>();
    em->SetRoot(btn);
    btn->NotifyMouseDown(Location());
    btn->NotifyMouseLeave();
    btn->NotifyMouseEnter();
    ASSERT_EQ(true, btn->IsHot());
    ASSERT_EQ(true, btn->IsPressed());
}

TEST(ButtonTests, WhenMouseLeftAndUppingOutside_StateIsNothing)
{
    auto em = make_shared<ElementManager>();
    auto btn = make_shared<Button>();
    em->SetRoot(btn);
    btn->NotifyMouseEnter();
    btn->NotifyMouseDown(Location());
    btn->NotifyMouseLeave();
    btn->NotifyMouseUp(Location());
    ASSERT_EQ(false, btn->IsHot());
    ASSERT_EQ(false, btn->IsPressed());
}

TEST(ButtonTests, WhenMouseLeftAndUppingOutside_NoClick)
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
    ASSERT_EQ(false, is_clicked);
}

TEST(ButtonTests, AfterMouseClicked_StateIsHot)
{
    auto em = make_shared<ElementManager>();
    auto btn = make_shared<Button>();
    em->SetRoot(btn);
    btn->NotifyMouseEnter();
    btn->NotifyMouseDown(Location());
    btn->NotifyMouseUp(Location());
    ASSERT_EQ(true, btn->IsHot());
    ASSERT_EQ(false, btn->IsPressed());
}

TEST(ButtonTests, WhenTouchDownAndUp_Click)
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
    ASSERT_EQ(true, isClicked);
}

TEST(ButtonTests, WhenTouchDownLeaveReturnAndUp_StateIsNotPressed)
{
    auto em = make_shared<ElementManager>();
    auto btn = make_shared<Button>();

    em->SetRoot(btn);
    btn->NotifyTouchDown(Location());
    btn->NotifyTouchLeave();
    btn->NotifyTouchEnter();
    btn->NotifyTouchUp(Location());

    ASSERT_EQ(false, btn->IsPressed());
    ASSERT_EQ(false, btn->IsHot());
}