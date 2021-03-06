#ifndef LISTENER_H
#define LISTENER_H

// System
#include <windows.h>

// Engine
#include "IGUIManager.h"

class CGUIWidget;



class CListener
{

	IGUIManager::EVENT_CALLBACK			_pfnCallback;

public:
										CListener();
	virtual 							~CListener();
	void								SetEventCallBack( IGUIManager::EVENT_CALLBACK pfnCallback );
	void								ExecuteCallBack( IGUIManager::ENUM_EVENT nEvent, CGUIWidget* pWidget, int x, int y);
	
};


#endif //LISTENER_H