#include "libgui/Common.h"
#include "libgui/Element.h"
#include "libgui/ElementManager.h"
#include "libgui/DrawingManager.h"
#include "libgui/Location.h"
#include "libgui/Layer.h"

namespace libgui
{
// Element Manager
ElementManager* Element::GetElementManager() const
{
    return _elementManager;
}

// Layer
Layer* Element::GetLayer() const
{
    return _layer;
}

// View Model
void Element::SetViewModel(shared_ptr<ViewModelBase> viewModel)
{
    _viewModel = viewModel;
}

shared_ptr<ViewModelBase> Element::GetViewModel()
{
    return _viewModel;
}

// Visual tree
shared_ptr<Element> Element::GetParent()
{
    return _parent;
}

shared_ptr<Element> Element::GetFirstChild()
{
    return _firstChild;
}

shared_ptr<Element> Element::GetLastChild()
{
    return _lastChild;
}

shared_ptr<Element> Element::GetPrevSibling()
{
    return _prevsibling;
}

shared_ptr<Element> Element::GetNextSibling()
{
    return _nextsibling;
}

void Element::RemoveChildren()
{
    // Recurse into children to thoroughly clean the element tree
    auto e = _firstChild;
    while (e != nullptr)
    {
        e->RemoveChildren();

        // Clean up pointers so that the class will be deleted
        e->_parent      = nullptr;
        e->_prevsibling = nullptr;
        auto next_e = e->_nextsibling;
        e->_nextsibling = nullptr;
        e = next_e;
    }

    _firstChild    = nullptr;
    _lastChild     = nullptr;
    _childrenCount = 0;
}

void Element::AddChild(shared_ptr<Element> element)
{
    if (_firstChild == nullptr)
    {
        _firstChild = element;
    }
    else
    {
        element->_prevsibling    = _lastChild;
        _lastChild->_nextsibling = element;
    }

    element->_parent = shared_from_this();
    _lastChild = element;

    _childrenCount++;
}

int Element::GetChildrenCount()
{
    return _childrenCount;
}

void Element::SetSingleChild(shared_ptr<Element> child)
{
    // Could be optimized to improve performance
    RemoveChildren();
    AddChild(child);
}

shared_ptr<Element> Element::GetSingleChild()
{
    if (_childrenCount == 1)
    {
        return _firstChild;
    }

    if (_childrenCount == 0)
    {
        return nullptr;
    }

    throw std::runtime_error("There is more than one child in this element");
}

// Cache Management
void Element::ClearCache(int cacheLevel)
{
    ClearElementCache(cacheLevel);

    // Recurse to children
    if (_firstChild)
    {
        for (auto e = _firstChild; e != nullptr; e = e->_nextsibling)
        {
            e->ClearCache(cacheLevel);
        }
    }
}

void Element::ClearElementCache(int cacheLevel)
{
    // This is intended to be overridden as needed for OS-specific needs.
}

// Arrangement
void Element::ResetArrangement()
{
    _left    = 0;
    _top     = 0;
    _right   = 0;
    _bottom  = 0;
    _centerX = 0;
    _centerY = 0;
    _width   = 0;
    _height  = 0;

    _isLeftSet    = false;
    _isTopSet     = false;
    _isRightSet   = false;
    _isBottomSet  = false;
    _isCenterXSet = false;
    _isCenterYSet = false;
    _isWidthSet   = false;
    _isHeightSet  = false;
}

void Element::SetSetViewModelCallback(function<void(shared_ptr<Element>)> setViewModelCallback)
{
    _setViewModelCallback = setViewModelCallback;
}

void Element::PrepareViewModel()
{
    if (_setViewModelCallback)
    {
        _setViewModelCallback(shared_from_this());
    }
    else
    {
        // By default the ViewModel is copied from the parent
        if (_parent)
        {
            _viewModel = _parent->_viewModel;
        }
    }
}

void Element::SetArrangeCallback(function<void(shared_ptr<Element>)> arrangeCallback)
{
    _arrangeCallback = arrangeCallback;
}

void Element::Arrange()
{
    if (_arrangeCallback)
    {
        _arrangeCallback(shared_from_this());
    }
    else
    {
        // By default each element stretches
        // to fill its parent
        SetLeft(_parent->GetLeft());
        SetTop(_parent->GetTop());
        SetRight(_parent->GetRight());
        SetBottom(_parent->GetBottom());
    }
}

void Element::ArrangeAndDraw()
{
    ResetArrangement();
    PrepareViewModel();
    Arrange();
    if (GetIsVisible())
    {
        if (_clipToBounds)
        {
            if (auto startClipping = DrawingManager::Get()->GetStartClippingCallback())
            {
                Rect4 r(GetLeft(), GetTop(), GetRight(), GetBottom());
                startClipping(r);
            }
        }

        Draw();
        if (_firstChild)
        {
            for (auto e = _firstChild; e != nullptr; e = e->_nextsibling)
            {
                e->ArrangeAndDraw();
            }
        }

        if (_clipToBounds)
        {
            if (auto stopClipping = DrawingManager::Get()->GetStopClippingCallback())
            {
                stopClipping();
            }
        }
    }
}

void Element::RearrangeAndRedrawHelper(const Rect4& redrawRegion)
{
    if (_parent)
    {
        _parent->RearrangeAndRedrawHelper(redrawRegion);
    }

    ResetArrangement();
    PrepareViewModel();
    Arrange();
}

void Element::UpdateLayersBelow(Layer* layer, const Rect4& redrawRegion)
{
    if (!layer)
    {
        return;
    }

    // Move to next lower layer as long as this layer's opaque region wouldn't hide the area below
    if (!layer->OpaqueContainsRegion(redrawRegion))
    {
        UpdateLayersBelow(layer->GetLayerBelow(), redrawRegion);
    }

    // Now from the lowest layer up, search for the elements in the region and update them
    std::deque<Element*> elementsFound;
    std::queue<Element*> elementsFoundQueue(elementsFound);
    layer->FindVisibleElements(redrawRegion, elementsFoundQueue);

    if (!elementsFound.empty())
    {
        for (auto& element: elementsFound)
        {
            element->Draw();
        }
    }
}

void Element::Update()
{
    auto wasVisible = GetIsVisible();
    BeginDirtyTracking();
    {
        ResetArrangement();
        PrepareViewModel();
        Arrange();
    }
    auto& redrawRegion = EndDirtyTracking();

    // If the redraw region is totally hidden both before
    // and after the rearrangement, we don't have to do anything
    if ((!wasVisible && !GetIsVisible()) ||
        CoveredByLayerAbove(redrawRegion))
    {
        return;
    }

    // Do the layer(s) below
    UpdateLayersBelow(GetLayer(), redrawRegion);

    // Work up the hierarchy of this layer
    if (_parent)
    {
        _parent->RearrangeAndRedrawHelper(redrawRegion);
    }

    // TODO: now continue with layers above

}

void Element::InitializeAll()
{
    // Initialize the current element
    InitializeThis();

    // Then initialize all children
    if (_firstChild)
    {
        for (auto e = _firstChild; e != nullptr; e = e->_nextsibling)
        {
            e->InitializeAll();
        }
    }

}

void Element::InitializeThis()
{
    // Default behavior
    if (_parent)
    {
        // Copy the element manager from the parent
        _elementManager = _parent->_elementManager;

        // Copy the layer from the parent
        _layer = _parent->_layer;
    }
}

void Element::SetIsVisible(bool isVisible)
{
    _isVisible = isVisible;
}

bool Element::GetIsVisible()
{
    return _isVisible;
}

void Element::SetIsEnabled(bool isEnabled)
{
    _isEnabled = isEnabled;
}

bool Element::GetIsEnabled()
{
    return _isEnabled;
}

void Element::SetClipToBounds(bool clipToBounds)
{
    _clipToBounds = clipToBounds;
}

bool Element::GetClipToBounds()
{
    return _clipToBounds;
}

void Element::SetConsumesInput(bool consumesInput)
{
    _consumesInput = consumesInput;
}

bool Element::GetConsumesInput()
{
    return _consumesInput;
}

void Element::SetLeft(double left)
{
    _isLeftSet = true;
    _left      = left;
}

void Element::SetTop(double top)
{
    _isTopSet = true;
    _top      = top;
}

void Element::SetRight(double right)
{
    _isRightSet = true;
    _right      = right;
}

void Element::SetBottom(double bottom)
{
    _isBottomSet = true;
    _bottom      = bottom;
}

void Element::SetCenterX(double centerX)
{
    _isCenterXSet = true;
    _centerX      = centerX;
}

void Element::SetCenterY(double centerY)
{
    _isCenterYSet = true;
    _centerY      = centerY;
}

void Element::SetWidth(double width)
{
    _isWidthSet = true;
    _width      = width;
}

void Element::SetHeight(double height)
{
    _isHeightSet = true;
    _height      = height;
}

HPixels Element::GetLeft()
{
    if (!_isLeftSet)
    {
        if (_isWidthSet)
        {
            if (_isRightSet)
            {
                _left = _right - _width;
            }
            else if (_isCenterXSet)
            {
                _left = _centerX - (_width / 2);
            }
        }
        _isLeftSet = true;
    }
    return HPixels(_left, _elementManager->GetDpiX());
}

VPixels Element::GetTop()
{
    if (!_isTopSet)
    {
        if (_isHeightSet)
        {
            if (_isBottomSet)
            {
                _top = _bottom - _height;
            }
            else if (_isCenterYSet)
            {
                _top = _centerY - (_height / 2);
            }
        }
        _isTopSet = true;
    }
    return VPixels(_top, _elementManager->GetDpiY());
}

HPixels Element::GetRight()
{
    if (!_isRightSet)
    {
        if (_isWidthSet)
        {
            if (_isLeftSet)
            {
                _right = _left + _width;
            }
            else if (_isCenterXSet)
            {
                _right = _centerX + (_width / 2);
            }
        }
        _isRightSet = true;
    }
    return HPixels(_right, _elementManager->GetDpiX());
}

VPixels Element::GetBottom()
{
    if (!_isBottomSet)
    {
        if (_isHeightSet)
        {
            if (_isTopSet)
            {
                _bottom = _top + _height;
            }
            else if (_isCenterYSet)
            {
                _bottom = _centerY + (_height / 2);
            }
        }
        _isBottomSet = true;
    }
    return VPixels(_bottom, _elementManager->GetDpiY());
}

HPixels Element::GetCenterX()
{
    if (!_isCenterXSet)
    {
        if (_isLeftSet && _isRightSet)
        {
            _centerX = _left + (_right - _left) / 2;
        }
        else if (_isLeftSet && _isWidthSet)
        {
            _centerX = _left + (_width / 2);
        }
        else if (_isRightSet && _isWidthSet)
        {
            _centerX = _right - (_width / 2);
        }
        _isCenterXSet = true;
    }
    return HPixels(_centerX, _elementManager->GetDpiX());
}

VPixels Element::GetCenterY()
{
    if (!_isCenterYSet)
    {
        if (_isTopSet && _isBottomSet)
        {
            _centerY = _top + (_bottom - _top) / 2;
        }
        else if (_isTopSet && _isHeightSet)
        {
            _centerY = _top + (_height / 2);
        }
        else if (_isBottomSet && _isHeightSet)
        {
            _centerY = _bottom - (_height / 2);
        }
        _isCenterYSet = true;
    }
    return VPixels(_centerY, _elementManager->GetDpiY());
}

HPixels Element::GetWidth()
{
    if (!_isWidthSet)
    {
        if (_isLeftSet && _isRightSet)
        {
            _width = _right - _left;
        }
        _isWidthSet = true;
    }
    return HPixels(_width, _elementManager->GetDpiX());
}

VPixels Element::GetHeight()
{
    if (!_isHeightSet)
    {
        if (_isTopSet && _isBottomSet)
        {
            _height = _bottom - _top;
        }
        _isHeightSet = true;
    }
    return VPixels(_height, _elementManager->GetDpiY());
}

// Drawing
void Element::Draw()
{
    if (_drawCallback)
    {
        _drawCallback(shared_from_this());
    }
    else
    {
        // By default no drawing takes place
    }
}

void Element::SetDrawCallback(function<void(shared_ptr<Element>)> drawCallback)
{
    _drawCallback = drawCallback;
}

// Hit testing
ElementQueryInfo Element::GetElementAtPoint(Point point)
{
    return GetElementAtPointHelper(point, false);
}

ElementQueryInfo Element::GetElementAtPointHelper(Point point, bool hasDisabledAncestor)
{
    if (!GetIsVisible() || !GetConsumesInput())
    {
        return ElementQueryInfo();
    }

    if (_firstChild)
    {
        auto childrenHaveDisabledAncestor = hasDisabledAncestor || !GetIsEnabled();

        for (auto e = _lastChild; e != nullptr; e = e->_prevsibling)
        {
            auto elementQueryInfo = e->GetElementAtPointHelper(point, childrenHaveDisabledAncestor);
            if (elementQueryInfo.FoundElement())
            {
                return elementQueryInfo;
            }
        }
    }

    if (point.X >= GetLeft() && point.X <= GetRight() &&
        point.Y >= GetTop() && point.Y <= GetBottom())
    {
        return ElementQueryInfo(this, hasDisabledAncestor);
    }
    return ElementQueryInfo();
}

void Element::FindVisibleElements(const Rect4& region, std::queue<Element*>& results)
{
    // This is a plain old brute force algorithm to search the entire hierarchy and
    // find matches.

    if (!GetIsVisible())
    {
        return;
    }

    // Thanks to http://stackoverflow.com/a/306332/4307047 for the rectangle intersection logic
    if (region.left < GetRight() && region.right > GetLeft() &&
        region.top > GetBottom() && region.bottom < GetTop())
    {
        results.push(this);
    }

    if (_firstChild)
    {
        for (auto e = _firstChild; e != nullptr; e = e->_nextsibling)
        {
            e->FindVisibleElementsHelper(region, results);
        }
    }
}

Element::~Element()
{
}

void Element::SetLeft(Inches left)
{
    SetLeft(double(left) * _elementManager->GetDpiX());
}

void Element::SetTop(Inches top)
{
    SetTop(double(top) * _elementManager->GetDpiY());
}

void Element::SetRight(Inches right)
{
    SetRight(double(right) * _elementManager->GetDpiX());
}

void Element::SetBottom(Inches bottom)
{
    SetBottom(double(bottom) * _elementManager->GetDpiY());
}

void Element::SetCenterX(Inches centerX)
{
    SetCenterX(double(centerX) * _elementManager->GetDpiX());
}

void Element::SetCenterY(Inches centerY)
{
    SetCenterY(double(centerY) * _elementManager->GetDpiY());
}

void Element::SetWidth(Inches width)
{
    SetWidth(double(width) * _elementManager->GetDpiX());
}

void Element::SetHeight(Inches height)
{
    SetHeight(double(height) * _elementManager->GetDpiY());
}

HPixels Element::GetHPixels(Inches in)
{
    return HPixels(in, _elementManager->GetDpiX());
}

VPixels Element::GetVPixels(Inches in)
{
    return VPixels(in, _elementManager->GetDpiY());
}

ElementQueryInfo::ElementQueryInfo(Element* ElementAtPoint, bool HasDisabledAncestor)
    : ElementAtPoint(ElementAtPoint), HasDisabledAncestor(HasDisabledAncestor)
{
}

ElementQueryInfo::ElementQueryInfo()
    : ElementAtPoint(nullptr), HasDisabledAncestor(false)
{
}

bool ElementQueryInfo::FoundElement()
{
    return bool(ElementAtPoint);
}

bool Element::ThisOrAncestors(const std::function<bool(Element*)>& predicate)
{
    Element* current = this;
    do
    {
        if (predicate(current))
        {
            return true;
        }

        // Move to the next ancestor
        current = current->_parent.get();
    }
    while (current);

    return false;
}

void Element::SetVisualBounds(const Rect4& bounds)
{
    _visualBounds = bounds;
}

const boost::optional<Rect4>& Element::GetVisualBounds()
{
    return _visualBounds;
}

const Rect4 Element::GetTotalBounds()
{
    if (_visualBounds)
    {
        auto& visualBounds = _visualBounds.get();
        return Rect4(GetLeft() + visualBounds.left,
                     GetTop() + visualBounds.top,
                     GetRight() + visualBounds.right,
                     GetBottom() + visualBounds.bottom);
    }
    else
    {
        return Rect4(GetLeft(), GetTop(), GetRight(), GetBottom());
    }
}

void Element::BeginDirtyTracking()
{
    // Store the dirty bounds for later
    _dirtyBounds = GetTotalBounds();
}

const Rect4& Element::EndDirtyTracking()
{
    auto currentBounds = GetTotalBounds();

    // Union the original bounds with the current bounds
    // This is so that if the element has moved since
    // we began dirty tracking, we'll be able to get
    // the whole dirty region, both the region that must
    // be cleared and also the region that must be drawn

    if (currentBounds.left < _dirtyBounds.left)
    {
        _dirtyBounds.left = currentBounds.left;
    }
    if (currentBounds.top < _dirtyBounds.top)
    {
        _dirtyBounds.top = currentBounds.top;
    }
    if (currentBounds.right > _dirtyBounds.right)
    {
        _dirtyBounds.right = currentBounds.right;
    }
    if (currentBounds.bottom > _dirtyBounds.bottom)
    {
        _dirtyBounds.bottom = currentBounds.bottom;
    }

    return _dirtyBounds;
}

bool Element::CoveredByLayerAbove(const Rect4& region)
{
    Layer* current = GetLayer();
    while ((current = current->GetLayerAbove()))
    {
        if (current->OpaqueContainsRegion(region))
        {
            return true;
        }
    }

    return false;
}
}

