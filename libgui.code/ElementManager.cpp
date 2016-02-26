#include "libgui/Common.h"
#include "libgui/ElementManager.h"
#include "libgui/Location.h"
#include "libgui/Layer.h"

namespace libgui
{

Layer* ElementManager::AddLayerAbove(Layer* existing, const string& typeName)
{
    LayerList::iterator existingIter = _layers.end();

    if (existing)
    {
        for (existingIter = _layers.begin(); existingIter != _layers.end(); ++existingIter)
        {
            auto existingLayer = *existingIter;
            if (existingLayer.get() == existing)
            {
                // We've found the insertion location
                break;
            }
        }
    }

    Layer* lower = nullptr;
    LayerList::iterator insertionIter = _layers.end();

    if (_layers.end() == existingIter)
    {
        // No matching lower layer specified, so add to the end
        if (!_layers.empty())
        {
            lower = _layers.back().get();
        }
    }
    else
    {
        // We found a matching lower layer
        lower = existing;

        // Have to move next to be ready for insertion
        insertionIter = existingIter;
        ++insertionIter;
    }

    Layer* higher = nullptr;

    if (lower)
    {
        higher = lower->_layerAbove;
    }

    Layer* adding = new Layer();

    adding->_elementManager = this;
    adding->_layer          = adding;
    adding->_typeName       = typeName;
    adding->_layerBelow     = lower;
    adding->_layerAbove     = higher;

    if (lower)
    {
        lower->_layerAbove = adding;
    }
    if (higher)
    {
        higher->_layerBelow = adding;
    }


    if (_layers.end() == insertionIter)
    {
        _layers.push_back(std::shared_ptr<Layer>(adding));
    }
    else
    {
        _layers.insert(insertionIter, std::shared_ptr<Layer>(adding));
    }

    return adding;
}

Layer* ElementManager::AddLayerBelow(Layer* existing, const string& typeName)
{
    LayerList::iterator existingIter = _layers.end();

    if (existing)
    {
        for (existingIter = _layers.begin(); existingIter != _layers.end(); ++existingIter)
        {
            auto existingLayer = *existingIter;
            if (existingLayer.get() == existing)
            {
                // We've found the insertion location
                break;
            }
        }
    }

    Layer* higher = nullptr;

    if (_layers.end() == existingIter)
    {
        // No matching lower layer specified, so add to the end
        if (!_layers.empty())
        {
            higher = _layers.front().get();
        }
    }
    else
    {
        // We found a matching higher layer
        higher = existing;
    }


    Layer* lower = nullptr;

    if (higher)
    {
        lower = higher->_layerBelow;
    }

    Layer* adding = new Layer();

    adding->_elementManager = this;
    adding->_layer          = adding;
    adding->_typeName       = typeName;
    adding->_layerBelow     = lower;
    adding->_layerAbove     = higher;

    if (higher)
    {
        higher->_layerBelow = adding;
    }
    if (lower)
    {
        lower->_layerAbove = adding;
    }

    if (_layers.end() == existingIter)
    {
        _layers.push_back(std::shared_ptr<Layer>(adding));
    }
    else
    {
        _layers.insert(existingIter, std::shared_ptr<Layer>(adding));
    }

    return adding;
}

void ElementManager::RemoveLayer(Layer* layer)
{
    layer->Update(Element::UpdateType::Removing);

    layer->VisitThisAndDescendents(
        [](Element* e)
        {
            e->_isDetached = true;
        });

    auto layerBelow = layer->_layerBelow;
    auto layerAbove = layer->_layerAbove;

    // Most likely the higher layers will be removed before lower layers so
    // we use a reverse iterator to search those layers first
    for (auto it = _layers.rbegin(); it != _layers.rend(); ++it)
    {
        if ((*it).get() == layer)
        {
            ++it; // See http://www.drdobbs.com/cpp/three-guidelines-for-effective-iterator/184401406?pgno=3
            _layers.erase(it.base());

            // Update the layer pointers
            if (layerBelow)
            {
                layerBelow->_layerAbove = layerAbove;
            }
            if (layerAbove)
            {
                layerAbove->_layerBelow = layerBelow;
            }

            break;
        }
    }
}

void ElementManager::UpdateEverything()
{
    for (auto& layer: _layers)
    {
        layer->Update(Element::UpdateType::Everything);
    }
}

void ElementManager::SetSystemCaptureCallback(const function<void(bool)>& systemCaptureCallback)
{
    _systemCaptureCallback = systemCaptureCallback;
}

void ElementManager::NotifyNewPoint(InputId inputId, Point point)
{
    auto input = GetInput(inputId);

    // Loop through the layers from the top to the bottom
    ElementQueryInfo elementQueryInfo;

    for (auto layerIter = _layers.rbegin(); layerIter != _layers.rend(); ++layerIter)
    {
        auto& layer = *layerIter;
        elementQueryInfo = layer->GetElementAtPoint(point);
        if (elementQueryInfo.FoundElement())
        {
            break;
        }
    }

    input->NotifyNewPoint(point, elementQueryInfo);
}

void ElementManager::NotifyDown(InputId inputId)
{
    auto input = GetInput(inputId);
    input->NotifyDown();
}

void ElementManager::NotifyUp(InputId inputId)
{
    auto input = GetInput(inputId);
    input->NotifyUp();
}

const Point& ElementManager::GetCurrentPoint(InputId inputId)
{
    auto input = GetInput(inputId);
    return input->GetPoint();
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
    #ifdef DBG
    printf("Pushing clip (%f, %f, %f, %f)\n",
           clip.left, clip.top, clip.right, clip.bottom);
    fflush(stdout);
    #endif

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

void ElementManager::ClearUpdateTracking()
{
    _updatedElements.clear();
}

bool ElementManager::TryBeginUpdate(Element* element)
{
    for (auto& e: _updatedElements)
    {
        if (e == element)
        {
            return false;
        }
    }

    _updatedElements.push_back(element);
    return true;
}

const Size& ElementManager::GetSize() const
{
    return _size;
}

void ElementManager::SetSize(const Size& size)
{
    _size = size;
}

double ElementManager::GetWidth() const
{
    return _size.width;
}

double ElementManager::GetHeight() const
{
    return _size.height;
}
}
