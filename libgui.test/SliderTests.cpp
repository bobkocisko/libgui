//
// Created by sjm-kociskobd on 12/22/15.
//

#include <libgui/Slider.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
using namespace libgui;
using namespace std;

using ::testing::Return;


TEST(SliderTests, WhenPointerDown_StateIsEngaged)
{
    auto slider = make_shared<Slider>();
    slider->Init();
    auto thumb = slider->GetThumb();

    bool updateScreen;
    thumb->NotifyInput(InputType::Pointer, InputAction::EnterReleased, Point(), updateScreen);
    thumb->NotifyInput(InputType::Pointer, InputAction::Push, Point(), updateScreen);

    ASSERT_EQ(Slider::Thumb::State::Engaged, thumb->GetState());
}