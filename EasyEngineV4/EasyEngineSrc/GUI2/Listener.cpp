#define LISTENER_CPP

#include "listener.h"



CListener::CListener(void)
:
_pfnCallback(NULL)
{
}



CListener::~CListener(void)
{
}


void CListener::SetEventCallBack( IGUIManager::EVENT_CALLBACK pfnCallback )
{
	_pfnCallback = pfnCallback;
}


void CListener::ExecuteCallBack( IGUIManager::ENUM_EVENT nEvent )
{
	if (_pfnCallback)
		_pfnCallback( nEvent );
}

