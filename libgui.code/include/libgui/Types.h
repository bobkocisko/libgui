//
// Created by sjm-kociskobd on 2/3/16.
//

#pragma once

#include <boost/operators.hpp>

namespace libgui
{

// @formatter:off
class Inches:
    boost::additive<Inches,
    boost::multiplicative<Inches,
    boost::multipliable<Inches, double,
    boost::dividable<Inches, double>>>>
{
public:
    explicit constexpr Inches(double value)
        : value(value) {}

    explicit operator double() const;

    Inches& operator+=(const Inches& other);
    Inches& operator-=(const Inches& other);
    Inches& operator*=(const Inches& other);
    Inches& operator/=(const Inches& other);
    Inches& operator*=(const double multiplier);

private:
    double value;
};

// Create a constant that can be used to convert to inches as follows: 2.0 * inches
constexpr Inches inches(1.0);

class HPixels:
    boost::addable<HPixels, Inches,
    boost::subtractable<HPixels, Inches,
    boost::multipliable<HPixels, Inches,
    boost::dividable<HPixels, Inches>>>>
{
public:
    HPixels(double pixels, double dpiX);
    HPixels(Inches in, double dpiX);

    // Allow implicit conversions to double
    operator double() const;

    HPixels& operator+=(const Inches& other);
    HPixels& operator-=(const Inches& other);
    HPixels& operator*=(const Inches& other);
    HPixels& operator/=(const Inches& other);

private:
    double pixels;
    double dpiX;
};

class VPixels:
    boost::addable<VPixels, Inches,
    boost::subtractable<VPixels, Inches,
    boost::multipliable<VPixels, Inches,
    boost::dividable<VPixels, Inches>>>>
{
public:
    VPixels(double pixels, double dpiY);
    VPixels(Inches in, double dpiY);

    // Allow implicit conversions to double
    operator double() const;

    VPixels& operator+=(const Inches& other);
    VPixels& operator-=(const Inches& other);
    VPixels& operator*=(const Inches& other);
    VPixels& operator/=(const Inches& other);

private:
    double pixels;
    double dpiY;
};

// @formatter:on

}