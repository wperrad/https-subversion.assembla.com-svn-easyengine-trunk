#include "Player.h"
#include "NPCEntity.h"
#include "IGUIManager.h"

CPlayer::CPlayer(
	string sFileName, 
	IRessourceManager& oRessourceManager, 
	IRenderer& oRenderer, 
	IEntityManager* pEntityManager, 
	IFileSystem* pFileSystem, 
	ICollisionManager& oCollisionManager, 
	IGeometryManager& oGeometryManager,
	IGUIManager& oGUIManager) :
	CMobileEntity(sFileName, oRessourceManager, oRenderer, pEntityManager, pFileSystem, oCollisionManager, oGeometryManager),
	m_oGUIManager(oGUIManager),
	m_pPlayerWindow(NULL)
{
	m_pPlayerWindow = m_oGUIManager.CreatePlayerWindow(600, 800);
}


CPlayer::~CPlayer()
{
}


void CPlayer::Action()
{
	CNPCEntity* pSpeaker = NULL;
	float fMinDistance = 200.f;
	for (int i = 0; i < m_pParent->GetChildCount(); i++) {
		CNPCEntity* pNPC = dynamic_cast<CNPCEntity*>(m_pParent->GetChild(i));
		if (pNPC) {
			float d = GetDistance(pNPC);
			if ((d < 150.f) && (fMinDistance > d)) {
				fMinDistance = d;
				pSpeaker = pNPC;
			}
		}
	}
	if (pSpeaker) {
		m_oGUIManager.SetCurrentWindow(m_oGUIManager.GetTopicsWindow());
		m_oGUIManager.SetGUIMode(true);
	}
}

void CPlayer::DisplayPlayerWindow(bool bDisplay)
{
	m_oGUIManager.SetCurrentWindow(bDisplay ? m_pPlayerWindow : NULL);
	m_oGUIManager.SetGUIMode(bDisplay);
}