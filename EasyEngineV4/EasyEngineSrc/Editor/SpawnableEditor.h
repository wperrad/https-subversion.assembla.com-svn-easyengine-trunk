#pragma once

#include "IEventDispatcher.h"

class CSpawnableEditor
{
public:
	void OnMouseEventCallback(CPlugin* plugin, IEventDispatcher::TMouseEvent e, int x, int y);
};