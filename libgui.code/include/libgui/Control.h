#pragma once 
#include "Element.h"
#include "Location.h"
#include "InputAction.h"
#include "InputType.h"
#include "Point.h"


namespace libgui
{
class Input;

class Control: public Element
{
public:
    // Called before NotifyInput and can optionally prevent the sending of NotifyInput event
    virtual void PreNotifyInput(InputAction
                                inputAction,
                                InputType inputType,
                                Point point,
                                bool& updateScreen,
                                bool& notify);

    // Called when an input action occurs.  See ElementManager for details of when this is called.
    virtual void NotifyInput(InputAction inputAction,
                             InputType inputType,
                             Point point,
                             bool& updateScreen);

    // State
    const bool& GetIsEnabled() const;
    void SetIsEnabled(bool isEnabled);

    // Input Management

    bool HasActiveInput() const;
    void SetHasActiveInput(bool hasActiveInput);

// Implementation
    void ResetArrangement() override;

private:
    bool _isEnabled      = true;
    bool _hasActiveInput = false;

};
}
