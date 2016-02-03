//
// Created by sjm-kociskobd on 10/8/15.
//

#pragma once

#include "Length.h"

class Location
{
public:

    Location();

    Location(const Length& x, const Length& y);
    Location(Length& x, Length& y);

    Length x;
    Length y;
};
