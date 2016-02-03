//
// Created by sjm-kociskobd on 2/3/16.
//

#include "libgui/Types.h"

namespace libgui
{

Inches::operator double() const
{
    return value;
}

Inches& Inches::operator+=(const Inches& other)
{
    value += other.value;
    return *this;
}

Inches& Inches::operator-=(const Inches& other)
{
    value -= other.value;
    return *this;
}

Inches& Inches::operator*=(const Inches& other)
{
    value *= other.value;
    return *this;
}

Inches& Inches::operator/=(const Inches& other)
{
    value /= other.value;
    return *this;
}

Inches& Inches::operator*=(const double multiplier)
{
    value *= multiplier;
    return *this;
}

Inches& Inches::operator/=(const double divisor)
{
    value /= divisor;
    return *this;
}

HPixels::HPixels(Inches in, double dpiX)
    : pixels(double(in) * dpiX), dpiX(dpiX)
{
}

HPixels::HPixels(double pixels, double dpiX)
    : pixels(pixels), dpiX(dpiX)
{
}

VPixels::VPixels(Inches in, double dpiY)
    : pixels(double(in) * dpiY), dpiY(dpiY)
{
}

VPixels::VPixels(double pixels, double dpiY)
    : pixels(pixels), dpiY(dpiY)
{
}

HPixels::operator double() const
{
    return pixels;
}

VPixels::operator double() const
{
    return pixels;
}

HPixels& HPixels::operator+=(const Inches& other)
{
    pixels += (double(other) * dpiX);
    return *this;
}

HPixels& HPixels::operator-=(const Inches& other)
{
    pixels -= (double(other) * dpiX);
    return *this;
}

HPixels& HPixels::operator*=(const Inches& other)
{
    pixels *= (double(other) * dpiX);
    return *this;
}

HPixels& HPixels::operator/=(const Inches& other)
{
    pixels /= (double(other) * dpiX);
    return *this;
}

VPixels& VPixels::operator+=(const Inches& other)
{
    pixels += (double(other) * dpiY);
    return *this;
}

VPixels& VPixels::operator-=(const Inches& other)
{
    pixels -= (double(other) * dpiY);
    return *this;
}

VPixels& VPixels::operator*=(const Inches& other)
{
    pixels *= (double(other) * dpiY);
    return *this;
}

VPixels& VPixels::operator/=(const Inches& other)
{
    pixels /= (double(other) * dpiY);
    return *this;
}
}