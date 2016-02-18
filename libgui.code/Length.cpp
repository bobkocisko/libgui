//
// Created by Bob Kocisko on 10/8/15.
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

Length::Length(const libgui::HPixels& hPixels)
    : Length(double(hPixels))
{

}

Length::Length(libgui::VPixels& vPixels)
    : Length(double(vPixels))
{
}

Length::Length(const libgui::VPixels& vPixels)
    : Length(double(vPixels))
{

}
