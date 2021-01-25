#pragma once
#include "MobileEntity.h"

class IGUIWindow;

class CPlayer :	public CMobileEntity, public IPlayer
{
public:
	CPlayer(
		string sFileName, 
		IRessourceManager& oRessourceManager, 
		IRenderer& oRenderer, 
		IEntityManager* pEntityManager, 
		IFileSystem* pFileSystem, 
		ICollisionManager& oCollisionManager, 
		IGeometryManager& oGeometryManager,
		IGUIManager& oGUIManager);
	virtual ~CPlayer();

	void	Action();
	void	ToggleDisplayPlayerWindow();
	void	Update();


protected:
	IGUIManager&	m_oGUIManager;
	IGUIWindow*		m_pPlayerWindow;
};

