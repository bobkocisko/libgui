#pragma once 
#include "Element.h"
#include "Control.h"
#include "Input.h"
#include <vector>

namespace libgui
{
	class ElementManager : public std::enable_shared_from_this<ElementManager>
	{
	public:
		void SetRoot(shared_ptr<Element>);

		shared_ptr<Element> GetRoot();

        bool NotifyNewPoint(InputId inputId, Point point);
        bool NotifyDown(InputId inputId);
        bool NotifyUp(InputId inputId);

		const function<void(bool)>& GetSystemCaptureCallback() const;
		void SetSystemCaptureCallback(const function<void(bool)>& systemCaptureCallback);

        // For debugging purposes
        const vector<Input*>& GetActiveInputs() const;

    private:
		std::vector<Input*> _activeInputs;

		shared_ptr<Element>  _root = nullptr;

		function<void(bool)> _systemCaptureCallback;
        Input* GetInput(const InputId& inputId);
    };
}
