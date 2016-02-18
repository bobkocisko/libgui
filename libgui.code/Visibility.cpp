//
// Created by Bob Kocisko on 10/8/15.
//

#include "libgui/Visibility.h"
bool Visibility::GetIsVisible()
{
    // For simplicity we will consider any non-zero value to be visible.
    return GetValue() != 0.0;
}
