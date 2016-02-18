//
// Created by Bob Kocisko on 12/18/15.
//

#pragma once

struct Point
{
    double X;
    double Y;

    bool operator==(const Point& other)
    {
        return (X == other.X &&
                Y == other.Y);
    }

    bool operator!=(const Point& other)
    {
        return !(*this == other);
    }
};