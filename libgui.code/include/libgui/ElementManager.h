#pragma once 
#include "Element.h"
#include "Control.h"
#include "Input.h"
#include <vector>
#include <list>

class Layer;

namespace libgui
{
class ElementManager: public std::enable_shared_from_this<ElementManager>
{
public:
    void AddLayer(const std::unique_ptr<Layer>&& layer);

    bool NotifyNewPoint(InputId inputId, Point point);
    bool NotifyDown(InputId inputId);
    bool NotifyUp(InputId inputId);

    const function<void(bool)>& GetSystemCaptureCallback() const;
    void SetSystemCaptureCallback(const function<void(bool)>& systemCaptureCallback);

    // DPI: The number of pixels per inch, used to support setting element values in inches
    double GetDpiX() const;
    // DPI: The number of pixels per inch, used to support setting element values in inches.
    // If you need a per-monitor DPI setting you need to create a separate ElementManager for each monitor.
    void   SetDpiX(double dpi);
    // DPI: The number of pixels per inch, used to support setting element values in inches
    double GetDpiY() const;
    // DPI: The number of pixels per inch, used to support setting element values in inches.
    // If you need a per-monitor DPI setting you need to create a separate ElementManager for each monitor.
    void   SetDpiY(double dpiY);

    // For debugging purposes
    const vector<Input*>& GetActiveInputs() const;
    void EnableDebugLogging();

private:
    std::vector<Input*>               _activeInputs;
    std::list<std::unique_ptr<Layer>> _layers;
    function<void(bool)>              _systemCaptureCallback;
    bool                              _isDebugLoggingEnabled;
    double                            _dpiX = 96.0;
    double                            _dpiY = 96.0;

    Input* GetInput(const InputId& inputId);
};
}
