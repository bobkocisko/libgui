#include "libgui/Common.h"
#include "libgui/ElementManager.h"
#include "libgui/Location.h"
#include "libgui/Layer.h"

namespace libgui
{
void ElementManager::AddLayer(const std::unique_ptr<Layer>&& layer)
{
    layer->_elementManager = this;
    layer->_layer          = layer.get();

    _layers.push_back(std::move(layer));
}

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

    // Loop through the layers from the top to the bottom
    ElementQueryInfo elementQueryInfo;
    for (auto        layerIter = _layers.rbegin(); layerIter != _layers.rend(); ++layerIter)
    {
        auto& layer = *layerIter;
        elementQueryInfo = layer->GetElementAtPoint(point);
        if (elementQueryInfo.FoundElement())
        {
            break;
        }
    }

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

void ElementManager::SetPushClipCallback(const function<void(Rect4)>& callback)
{
    _pushClipCallback = callback;
}

void ElementManager::SetPopClipCallback(const function<void()>& callback)
{
    _popClipCallback = callback;
}

void ElementManager::PushClip(const Rect4& clip)
{
    if (_pushClipCallback)
    {
        _pushClipCallback(clip);
    }
}

void ElementManager::PopClip()
{
    if (_popClipCallback)
    {
        _popClipCallback();
    }
}
}
