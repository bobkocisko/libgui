#include "libgui/Common.h"
#include "libgui/ElementManager.h"
#include "libgui/Location.h"

namespace libgui
{
void ElementManager::SetRoot(shared_ptr<Element> element)
{
    _root = element;
    element->SetElementManager(shared_from_this());
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

bool ElementManager::NotifyMove(InputId inputId, Point point)
{
    auto input = GetInput(inputId);

    auto elementAtPoint = _root->GetElementAtPoint(point);

    return input->NotifyMove(point, elementAtPoint);

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
        _activeInputs[inputId] = result;
    }

    return result;
}
}
