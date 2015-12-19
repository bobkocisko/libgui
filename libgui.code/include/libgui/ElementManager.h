#pragma once 
#include "Element.h"
#include "Control.h"
#include "Input.h"
#include <list>

namespace libgui
{
	class ElementManager : public std::enable_shared_from_this<ElementManager>
	{
	public:
		void SetRoot(shared_ptr<Element>);

		shared_ptr<Element> GetRoot();

		bool NotifyMove(InputId inputId, Point point);
        bool NotifyDown(InputId inputId);
        bool NotifyUp(InputId inputId);

		const function<void(bool)>& GetSystemCaptureCallback() const;
		void SetSystemCaptureCallback(const function<void(bool)>& systemCaptureCallback);

	private:
		//std::list<ActiveControl*> _activeControls;
		std::vector<Input*> _activeInputs;

		shared_ptr<Element>  _root = nullptr;

		function<void(bool)> _systemCaptureCallback;
        Input* GetInput(const InputId& inputId);
    };
}
