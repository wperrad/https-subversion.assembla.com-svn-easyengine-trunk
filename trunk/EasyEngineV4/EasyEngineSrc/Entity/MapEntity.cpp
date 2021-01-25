#include "MapEntity.h"

#include "Scene.h"
#include "MobileEntity.h"
#include "LightEntity.h"
#include "ICameraManager.h"
#include "ICamera.h"
#include "EntityManager.h"

CMapEntity::CMapEntity(
	const string& sFileName,
	IRessourceManager& oRessourceManager,
	IRenderer& oRenderer,
	IEntityManager* pEntityManager,
	IGeometryManager& oGeometryManager,
	ICollisionManager& oCollisionManager,
	ICameraManager&	oCameraManager,
	bool bDuplicate) : 	
	CEntity(
		sFileName, 
		oRessourceManager, 
		oRenderer, 
		pEntityManager, 
		oGeometryManager, 
		oCollisionManager, 
		bDuplicate),
	m_oCameraManager(oCameraManager),
	m_sFirstPassShaderName("mapFirstPass"),
	m_sSecondPassShaderName("mapSecondPass3D")
{
	SetShader(m_oRenderer.GetShader(m_sSecondPassShaderName));
}

CMapEntity::~CMapEntity()
{
}

void CMapEntity::Link(CNode* pNode)
{
	CEntity::Link(pNode);
	IMesh* pMesh = static_cast<IMesh*>(m_pRessource);
	pMesh->SetTexture(m_pScene->GetMapTexture());
}

void CMapEntity::Update()
{
	CNode::Update();
	m_pScene->RenderMap();
	// second pass
	m_oRenderer.SetCurrentFBO(0);	
	m_oRenderer.SetModelMatrix(m_oWorldMatrix);
	m_oRenderer.CullFace(0);
	UpdateRessource();
	m_oRenderer.CullFace(1);
}