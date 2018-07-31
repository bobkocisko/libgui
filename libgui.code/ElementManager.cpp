#include "libgui/ElementManager.h"
#include "libgui/Location.h"
#include "libgui/Layer.h"
#include "libgui/ScopeExit.h"

namespace libgui
{

ElementManager::ElementManager()
 : _inUpdateCycle(false),
   _fuzzyTouchSize(Size(30, 30)) // default fuzzy touch size
{
}

void ElementManager::AddLayerAbove(std::shared_ptr<Layer> existing, std::shared_ptr<Layer> adding)
{
  LayerList::iterator existingIter = _layers.end();

  if (existing)
  {
    for (existingIter = _layers.begin(); existingIter != _layers.end(); ++existingIter)
    {
      auto existingLayer = *existingIter;
      if (existingLayer == existing)
      {
        // We've found the insertion location
        break;
      }
    }
  }

  std::shared_ptr<Layer> lower         = nullptr;
  LayerList::iterator    insertionIter = _layers.end();

  if (_layers.end() == existingIter)
  {
    // No matching lower layer specified, so add to the end
    if (!_layers.empty())
    {
      lower = _layers.back();
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

  std::shared_ptr<Layer> higher = nullptr;

  if (lower)
  {
    higher = lower->_layerAbove.lock();
  }

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
}

void ElementManager::AddLayerBelow(std::shared_ptr<Layer> existing, std::shared_ptr<Layer> adding)
{
  LayerList::iterator existingIter = _layers.end();

  if (existing)
  {
    for (existingIter = _layers.begin(); existingIter != _layers.end(); ++existingIter)
    {
      auto existingLayer = *existingIter;
      if (existingLayer == existing)
      {
        // We've found the insertion location
        break;
      }
    }
  }

  std::shared_ptr<Layer> higher = nullptr;

  if (_layers.end() == existingIter)
  {
    // No matching lower layer specified, so add to the end
    if (!_layers.empty())
    {
      higher = _layers.front();
    }
  }
  else
  {
    // We found a matching higher layer
    higher = existing;
  }


  std::shared_ptr<Layer> lower = nullptr;

  if (higher)
  {
    lower = higher->_layerBelow.lock();
  }

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
}

void ElementManager::RemoveLayer(std::shared_ptr<Layer> layer)
{
  layer->Update(Element::UpdateType::Removing);

  // Allow subclasses to do additional cleanup
  layer->OnElementIsBeingRemoved();

  // Among the children of a layer, shared_ptrs hold circular locks in every direction
  // so until this is called nothing will be destructed
  layer->RemoveChildren(Element::UpdateWhenRemoving::No);

  // Allow the layer itself to be destructed
  layer->_layer = nullptr;

  // Remove callbacks which often capture shared pointers to other elements
  // which in turn can hold references to this element and thereby keep
  // each other alive artificially
  layer->_arrangeCallback = nullptr;
  layer->_drawCallback = nullptr;
  layer->_setViewModelCallback = nullptr;

  layer->SetIsDetached(true);

  auto layerBelow = layer->_layerBelow.lock();
  auto layerAbove = layer->_layerAbove.lock();

  // Most likely the higher layers will be removed before lower layers so
  // we use a reverse iterator to search those layers first
  for (auto it = _layers.rbegin(); it != _layers.rend(); ++it)
  {
    if ((*it) == layer)
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

void ElementManager::SetSystemCaptureCallback(const std::function<void(bool)>& systemCaptureCallback)
{
  _systemCaptureCallback = systemCaptureCallback;
}

void ElementManager::NotifyNewPoint(InputId inputId, Point point)
{
  if (inputId.IsPointer())
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
  else
  {
    // This is a touch input: we need to use a fuzzy zone for hit testing

    Size halfSize = Size(_fuzzyTouchSize.width / 2,
                         _fuzzyTouchSize.height / 2);
    FuzzyHitQuery query;
    Rect4 hitRect(point.X - halfSize.width, point.Y - halfSize.height,
                  point.X + halfSize.width, point.Y + halfSize.height);

    auto input = GetInput(inputId);

    // Loop through the layers from the top to the bottom.  Stop only if
    // we find an element that covers fifty percent of the fuzzy zone or
    // if the specified layer captures all fuzzy input at the borders.

    auto remainingLayers = _layers.size();
    for (auto layerIter = _layers.rbegin(); layerIter != _layers.rend(); ++layerIter)
    {
      --remainingLayers;

      auto& layer = *layerIter;
      bool anyIntersection = layer->GetElementInRect(hitRect, query);
      if (query.FoundFiftyPercent() ||
          (layer->GetCapturesAllIntersectingTouchInput() && anyIntersection))
      {
        break;
      }

      if (layer->GetConsumesInput() &&
          remainingLayers && anyIntersection)
      {
        // Shrink the hit rect to exclude the bounds of each layer before
        // moving to the next lower layer.  If trimming this leaves us with
        // an empty rect, then stop looking because we were entirely within the
        // layer and found no matches.

        auto origHitRect = hitRect;
        hitRect.ExcludeWith(layer->GetBounds());
        if (hitRect.IsEmpty())
        {
          break;
        }

        // Shrinking the hit rect did NOT leave it empty, so to fairly
        // compare with any existing element we need to adjust the current
        // percentage by the same amount as we shrank the area.  And if
        // doing so means that the found element is now fifty percent of
        // the new hit testing area, then it wins immediately.
        if (query.MaxMatchingElement.FoundElement())
        {
          auto sizePercent = hitRect.Area() / origHitRect.Area();
          query.MaxMatchingPercent /= sizePercent;
          if (query.FoundFiftyPercent())
          {
            break;
          }
        }
      }
    }

    // So at this point the query will contain no matching element or
    // else the one that matched the most.  Either way we want the input
    // to know about that.
    input->NotifyNewPoint(point, query.MaxMatchingElement);
  }
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

void ElementManager::SetFuzzyTouchSize(const Size& size)
{
  _fuzzyTouchSize = size;
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

const std::vector<Input*>& ElementManager::GetActiveInputs() const
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

void ElementManager::SetPushClipCallback(const std::function<void(const Rect4&)>& callback)
{
  _pushClipCallback = callback;
}

void ElementManager::SetPopClipCallback(const std::function<void()>& callback)
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

void ElementManager::UpdateOrAddPending(std::shared_ptr<Element> element,
                                        Element::UpdateType type)
{
  if (_inUpdateCycle)
  {
    _pendingUpdates.emplace_back(element, type);
    return;
  }

  // Beginning a new update cycle and end it when we're done
  _inUpdateCycle = true;
  ScopeExit scopeExit ([this]{ _inUpdateCycle = false; });

  element->UpdateHelper(type);

  // Before we finish the cycle, process and pop all remaining pending updates
  // (each update here might also add more pending updates which will also
  //  get processed)
  while (!_pendingUpdates.empty())
  {
    auto& update = _pendingUpdates.front();

    update.element->UpdateHelper(update.type);

    _pendingUpdates.pop_front();
  }
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

void ElementManager::NotifyControlIsBeingDestroyed(Control* control)
{
  for (int i = 1; i < _activeInputs.size(); ++i)
  {
    auto activeInput = _activeInputs[i];

    if (activeInput) // Don't expect all the input ids to have been created in order,
                     // especially if in touch mode (i == 1 will be empty)
    {
      activeInput->NotifyControlIsBeingDestroyed(control);
    }
  }
}
}
