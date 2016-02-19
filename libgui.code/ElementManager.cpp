#include "libgui/Common.h"
#include "libgui/ElementManager.h"
#include "libgui/Location.h"
#include "libgui/Layer.h"

namespace libgui
{
Layer* ElementManager::AddLayer()
{
    auto layer = new Layer();
    layer->_elementManager = this;
    layer->_layer          = layer;

    _layers.push_back(std::shared_ptr<Layer>(layer));
    return layer;
}

void ElementManager::ArrangeAndDrawAll()
{
    for (auto& layer: _layers)
    {
        layer->ArrangeAndDraw();

        AddToRedrawnRegion(layer->GetTotalBounds());
    }
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

void ElementManager::SetPushClipCallback(const function<void(const Rect4&)>& callback)
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

void ElementManager::ClearRedrawnRegion()
{
    _redrawnRegion = boost::none;
}

void ElementManager::AddToRedrawnRegion(const Rect4& region)
{
    if (_redrawnRegion)
    {
        // Expand the existing region to include this new region
        auto& currentRegion = _redrawnRegion.get();

        if (region.left < currentRegion.left)
        {
            currentRegion.left = region.left;
        }
        if (region.top < currentRegion.top)
        {
            currentRegion.top = region.top;
        }
        if (region.right > currentRegion.right)
        {
            currentRegion.right = region.right;
        }
        if (region.bottom > currentRegion.bottom)
        {
            currentRegion.bottom = region.bottom;
        }
    }
    else
    {
        // There isn't any region yet, so use the specified region as the only region
        _redrawnRegion = region;
    }
}

const boost::optional<Rect4>& ElementManager::GetRedrawnRegion()
{
    return _redrawnRegion;
}
}
