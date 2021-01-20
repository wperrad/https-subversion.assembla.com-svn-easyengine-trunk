#pragma once
#include "Entity.h"

class ICamera;

class CMapEntity : public CEntity
{
public:
	CMapEntity(
		const string& sFileName, 
		IRessourceManager& oRessourceManager, 
		IRenderer& oRenderer, 
		IEntityManager* pEntityManager, 
		IGeometryManager& oGeometryManager, 
		ICollisionManager& oCollisionManager,
		ICameraManager&	oCameraManager,
		bool bDuplicate = false);

	~CMapEntity();

	void				Update();



protected:

	void				RenderMap();
	void				CollectMapEntities(vector<CEntity*>& entities);
	void				DisplayEntities(vector<CEntity*>& entities);

private:
	ICameraManager&		m_oCameraManager;
	ITexture*			m_pMapTexture;
	int					m_nMapWidth;
	int					m_nMapHeight;
	const string		m_sFirstPassShaderName;
	const string		m_sSecondPassShaderName;
	ICamera*			m_pMapCamera;
};

