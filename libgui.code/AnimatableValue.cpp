//
// Created by Bob Kocisko on 10/8/15.
//

#include "libgui/AnimatableValue.h"
#include "libgui/Animation.h"

double AnimatableValue::GetValue() const
{
    return m_value;
}

void AnimatableValue::SetValue(double value)
{
    m_value = value;
}

AnimatableValue::operator double() const
{
    return m_value;
}

AnimatableValue::AnimatableValue(double value)
    : m_value(value)
{
}

AnimatableValue::~AnimatableValue()
{
    // Make sure to delete the circular reference from Animation
    // whenever we are being deleted
    if (m_activeAnimation)
    {
        m_activeAnimation->ClearAnimatableValue();
    }
}
const shared_ptr<Animation> &AnimatableValue::GetActiveAnimation() const
{
    return m_activeAnimation;
}

void AnimatableValue::SetActiveAnimation(const shared_ptr<Animation> &activeAnimation)
{
    m_activeAnimation = activeAnimation;
}