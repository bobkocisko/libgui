//
// Created by sjm-kociskobd on 10/8/15.
//

#pragma once

#include <memory>
#include "AnimatableValue.h"

using namespace std;

class Animation
{
public:
    void ClearAnimatableValue();

private:
    double m_CurrentValue;
    double m_TargetValue;
    double m_SourceValue;
    AnimatableValue *m_animatableValue;

    // TODO: Add timing information for tracking the animation
};



