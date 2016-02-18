//
// Created by sjm-kociskobd on 12/22/15.
//

#include <libgui/Slider.h>
#include <libgui/ElementManager.h>
#include "libgui/Layer.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace libgui;
using namespace std;

using ::testing::Return;


TEST(SliderTests, WhenPointerDown_StateIsEngaged)
{
    auto em     = make_shared<ElementManager>();
    auto root   = em->AddLayer();
    auto slider = make_shared<Slider>();
    root->AddChild(slider);
    root->InitializeAll();

    auto thumb = slider->GetThumb();

    bool updateScreen;
    thumb->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    thumb->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);

    ASSERT_EQ(Slider::Thumb::State::Engaged, thumb->GetState());
}

TEST(SliderTests, WhenTouchDown_StateIsEngaged)
{
    auto em     = make_shared<ElementManager>();
    auto root   = em->AddLayer();
    auto slider = make_shared<Slider>();
    root->AddChild(slider);
    root->InitializeAll();

    auto thumb = slider->GetThumb();

    bool updateScreen;
    thumb->NotifyInput(InputType::Touch, InputAction::EnterReleased, Point(), updateScreen);
    thumb->NotifyInput(InputType::Touch, InputAction::Push, Point(), updateScreen);

    ASSERT_EQ(Slider::Thumb::State::Engaged, thumb->GetState());
}

TEST(SliderTests, WhenTouchDragged_ValueChanges)
{
    auto em     = make_shared<ElementManager>();
    auto root   = em->AddLayer();
    auto slider = make_shared<Slider>();
    root->AddChild(slider);
    root->InitializeAll();

    auto thumb = slider->GetThumb();

    auto originalValue = slider->GetValue();

    bool updateScreen;
    thumb->NotifyInput(InputType::Touch, InputAction::EnterReleased, Point{1, 1}, updateScreen);
    thumb->NotifyInput(InputType::Touch, InputAction::Push, Point{1, 1}, updateScreen);

    ASSERT_EQ(originalValue, slider->GetValue());

    thumb->NotifyInput(InputType::Touch, InputAction::Move, Point{0, 1}, updateScreen);
    ASSERT_NE(originalValue, slider->GetValue());

}