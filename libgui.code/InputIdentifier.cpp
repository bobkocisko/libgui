//
// Created by Bob Kocisko on 12/18/15.
//

#include "libgui/InputIdentifier.h"

/*
 *
 * InputIdentifier
 *
 */

InputIdentifier::InputIdentifier(int inputId)
    : _inputId(inputId)
{
}

bool InputIdentifier::IsPointer()
{
    return PointerInputId == _inputId;
}

bool InputIdentifier::IsTouch()
{
    return _inputId > PointerInputId;
}

InputIdentifier::operator int() const
{
    return _inputId;
}

bool InputIdentifier::operator==(const InputIdentifier other)
{
    return _inputId == other._inputId;
}
