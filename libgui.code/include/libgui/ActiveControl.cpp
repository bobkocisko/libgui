//
// Created by sjm-kociskobd on 12/18/15.
//

#include "ActiveControl.h"

namespace libgui
{

ActiveControl::ActiveControl(Control* control)
    : _control(control)
{
}

const std::list<Input*>& ActiveControl::GetCapturedInputs()
{
    return _capturedInputs;
}

Control* ActiveControl::GetControl() const
{
    return _control;
}
}