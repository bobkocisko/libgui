#include "libgui/Common.h"
#include "libgui/ElementManager.h"
#include "libgui/Location.h"

namespace libgui
{
void ElementManager::SetRoot(shared_ptr<Element> element)
{
    _root = element;
    element->SetElementManager(this);
}

shared_ptr<Element> ElementManager::GetRoot()
{
    return _root;
}

// TODO: capture pointer and touch movements outside the current window as needed

const function<void(bool)>& ElementManager::GetSystemCaptureCallback() const
{
    return _systemCaptureCallback;
}

void ElementManager::SetSystemCaptureCallback(const function<void(bool)>& systemCaptureCallback)
{
    _systemCaptureCallback = systemCaptureCallback;
}

bool ElementManager::NotifyNewPoint(InputId inputId, Point point)
{
    auto input = GetInput(inputId);

    auto elementQueryInfo = _root->GetElementAtPoint(point);

    return input->NotifyNewPoint(point, elementQueryInfo);
}

bool ElementManager::NotifyDown(InputId inputId)
{
    auto input = GetInput(inputId);
    return input->NotifyDown();
}

bool ElementManager::NotifyUp(InputId inputId)
{
    auto input = GetInput(inputId);
    return input->NotifyUp();
}

Input* ElementManager::GetInput(const InputId& inputId)
{
    if (inputId >= _activeInputs.size())
    {
        _activeInputs.resize((size_t) inputId + 1, nullptr);
    }

    auto result = _activeInputs[inputId];
    if (nullptr == result)
    {
        result = new Input(inputId);

        if (_isDebugLoggingEnabled)
        {
            result->EnableDebugLogging();
        }

        _activeInputs[inputId] = result;
    }

    return result;
}

const vector<Input*>& ElementManager::GetActiveInputs() const
{
    return _activeInputs;
}

void ElementManager::EnableDebugLogging()
{
    _isDebugLoggingEnabled = true;
}

double ElementManager::GetDpiX() const
{
    return _dpiX;
}

void ElementManager::SetDpiX(double dpi)
{
    _dpiX = dpi;
}

double ElementManager::GetDpiY() const
{
    return _dpiY;
}

void ElementManager::SetDpiY(double dpiY)
{
    _dpiY = dpiY;
}
}
