//
// Created by sjm-kociskobd on 10/8/15.
//

#pragma once

#include "Length.h"

class Size
{
public:
    Size();

    Size(const Length& width, const Length& height);

    Size(Length& width, Length& height);

    Length width;
    Length height;
};
