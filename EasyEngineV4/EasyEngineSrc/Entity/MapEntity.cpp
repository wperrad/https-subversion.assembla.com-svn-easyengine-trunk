#include "MapEntity.h"
#include "Interface.h"
#include "Scene.h"
#include "MobileEntity.h"
#include "LightEntity.h"
#include "ICamera.h"
#include "EntityManager.h"

CMinimapEntity::CMinimapEntity(EEInterface& oInterface, const string& sFileName, bool bDuplicate) :
	CEntity(oInterface, sFileName, bDuplicate),
	m_oCameraManager(static_cast<ICameraManager&>(*oInterface.GetPlugin("CameraManager"))),
	m_sFirstPassShaderName("mapFirstPass"),
	m_sSecondPassShaderName("mapSecondPass3D")
{
	m_sTypeName = "MapEntity";
	SetShader(m_oRenderer.GetShader(m_sSecondPassShaderName));
}

CMinimapEntity::~CMinimapEntity()
{
}

void CMinimapEntity::Link(CNode* pNode)
{
	CEntity::Link(pNode);
	IMesh* pMesh = static_cast<IMesh*>(m_pRessource);
	pMesh->SetTexture(m_pScene->GetMinimapTexture());
}

void CMinimapEntity::Update()
{
	CNode::Update();
	m_pScene->RenderMinimap();
	// second pass
	m_oRenderer.SetCurrentFBO(0);	
	m_oRenderer.SetModelMatrix(m_oWorldMatrix);
	m_oRenderer.CullFace(0);
	UpdateRessource();
	m_oRenderer.CullFace(1);
}