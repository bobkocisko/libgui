#pragma once 
#include "Element.h"
#include "Control.h"
#include "Input.h"
#include <vector>
#include <list>

namespace libgui
{

class Layer;

class ElementManager: public std::enable_shared_from_this<ElementManager>
{
public:
    Layer* AddLayer();

    void ArrangeAndDrawAll();

    bool NotifyNewPoint(InputId inputId, Point point);
    bool NotifyDown(InputId inputId);
    bool NotifyUp(InputId inputId);

    void SetSystemCaptureCallback(const function<void(bool)>& systemCaptureCallback);
    void SetPushClipCallback(const function<void(const Rect4&)>& callback);
    void SetPopClipCallback(const function<void()>& callback);

    void PushClip(const Rect4& clip);
    void PopClip();

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
    std::list<std::shared_ptr<Layer>> _layers;
    function<void(bool)>              _systemCaptureCallback;
    bool                              _isDebugLoggingEnabled;
    double                            _dpiX = 96.0;
    double                            _dpiY = 96.0;
    function<void(const Rect4&)>      _pushClipCallback;
    function<void()>                  _popClipCallback;

    Input* GetInput(const InputId& inputId);
};
}
