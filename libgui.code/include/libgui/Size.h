//
// Created by sjm-kociskobd on 10/8/15.
//

#pragma once

#include "Length.h"

class Size
{
public:
    Size()
        : width(0), height(0)
    {
    }

    Size(Length width, Length height)
        : width(width), height(height)
    {
    }

    Length width;
    Length height;
};
