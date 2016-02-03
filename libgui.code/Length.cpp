//
// Created by sjm-kociskobd on 10/8/15.
//

#include "libgui/Length.h"

Length::Length(double value)
    : AnimatableValue(value)
{
}

Length::Length(libgui::HPixels& hPixels)
    : Length(double(hPixels))
{
}

Length::Length(libgui::VPixels& vPixels)
    : Length(double(vPixels))
{
}
