//
// Created by Bob Kocisko on 10/8/15.
//

#pragma once

#include <memory>
using namespace std;

class Animation;

class AnimatableValue
{
public:

    // Implicit conversions
    AnimatableValue(double value);
    operator double() const;

    double GetValue() const;
    void SetValue(double value);

    const shared_ptr<Animation> & GetActiveAnimation() const;
    void SetActiveAnimation(const shared_ptr<Animation> &activeAnimation);

    ~AnimatableValue();

private:
    double m_value;
    shared_ptr<Animation> m_activeAnimation;
};



