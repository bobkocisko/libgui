//
// Created by sjm-kociskobd on 12/18/15.
//

#pragma once

#include "Control.h"
#include "InputId.h"
#include <list>

namespace libgui
{

class Input;

class ActiveControl
{
public:
    ActiveControl(Control* control);

    Control* GetControl() const;

    const std::list<Input*>& GetCapturedInputs();

private:
    Control* _control;
    std::list<Input*> _capturedInputs;
};

}


