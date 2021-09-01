#pragma once
#include "MobileEntity.h"

class IGUIWindow;

class CPlayer :	public CMobileEntity, public IPlayer
{
public:
	CPlayer(EEInterface& oInterface, string sFileName);
	virtual ~CPlayer();

	void	Action();
	void	ToggleDisplayPlayerWindow();
	void	Update();


protected:
	IGUIManager&	m_oGUIManager;
	IGUIWindow*		m_pPlayerWindow;
};

