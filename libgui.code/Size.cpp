//
// Created by sjm-kociskobd on 2/3/16.
//

#include "libgui/Size.h"

Size::Size()
    : width(0), height(0)
{
}

Size::Size(const Length& width, const Length& height)
    : width(width), height(height)
{
}

Size::Size(Length& width, Length& height)
    : width(width), height(height)
{
}