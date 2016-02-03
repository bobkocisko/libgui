#include "libgui/Common.h"
#include "libgui/Element.h"
#include "libgui/ElementManager.h"
#include "libgui/DrawingManager.h"
#include "libgui/Location.h"

namespace libgui
{
// Element Manager
void Element::SetElementManager(ElementManager* elementManager)
{
    _elementManager = elementManager;
}

ElementManager* Element::GetElementManager() const
{
    return _elementManager;
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
    _isVisible = true;

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

void Element::ArrangeAndDraw(bool draw)
{
    ResetArrangement();
    PrepareViewModel();
    Arrange();
    if (draw && _isVisible)
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
                e->ArrangeAndDraw(draw);
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

void Element::SetClipToBounds(bool clipToBounds)
{
    _clipToBounds = clipToBounds;
}

bool Element::GetClipToBounds()
{
    return _clipToBounds;
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
Element* Element::GetElementAtPoint(Point point)
{
    if (_firstChild)
    {
        for (auto e = _lastChild; e != nullptr; e = e->_prevsibling)
        {
            auto elementAtPoint = e->GetElementAtPoint(point);
            if (elementAtPoint)
            {
                return elementAtPoint;
            }
        }
    }

    if (GetIsVisible() &&
        point.X >= GetLeft() && point.X <= GetRight() &&
        point.Y >= GetTop() && point.Y <= GetBottom())
    {
        return this;
    }
    return nullptr;
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
}

