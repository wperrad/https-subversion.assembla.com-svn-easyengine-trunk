#pragma once
#include "GUIWindow.h"

class CGUIManager;
class ITexture;

class CMinimapWindow :	public CGUIWindow
{
public:
	CMinimapWindow(IGUIManager* pGUIManager, IScene& oScene, IRessourceManager& oRessourceManager, IRenderer& oRenderer, int nWidth, int nHeight);
	~CMinimapWindow();

	void				Display();

protected:
	
private:
	CGUIManager*		m_pGUIManager;
	IScene&				m_oScene;
	IRenderer&			m_oRenderer;
	CGUIWidget			m_oMinimap;
};

