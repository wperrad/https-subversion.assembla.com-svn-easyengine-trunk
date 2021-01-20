#include "MapEntity.h"

#include "Scene.h"
#include "MobileEntity.h"
#include "LightEntity.h"
#include "ICameraManager.h"
#include "ICamera.h"

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
	m_pMapTexture(NULL),
	m_sFirstPassShaderName("mapFirstPass"),
	m_sSecondPassShaderName("mapSecondPass3D")
{
	SetShader(m_oRenderer.GetShader(m_sSecondPassShaderName));
	m_oRenderer.GetResolution(m_nMapWidth, m_nMapHeight);
	m_pMapTexture = m_oRessourceManager.CreateRenderTexture(m_nMapWidth, m_nMapHeight, m_sSecondPassShaderName);
	IMesh* pMesh = static_cast<IMesh*>(m_pRessource);
	pMesh->SetTexture(m_pMapTexture);
	m_pMapCamera = m_oCameraManager.GetCameraFromType(ICameraManager::T_MAP_CAMERA);
	m_pMapCamera->SetWorldPosition(0, 60000, 0);
	m_pMapCamera->Pitch(-90);
}

CMapEntity::~CMapEntity()
{
}


void CMapEntity::Update()
{
	CNode::Update();
	
	// first pass
	static bool doFirstPass = true;
	if (doFirstPass) {
		m_oRenderer.SetCurrentFBO(m_pMapTexture->GetFrameBufferObjectId());
		m_oRenderer.ClearColorBuffer(0, 0, 0, 1);
		static bool bRenderMap = true;
		if(bRenderMap)
			RenderMap();
	}

	// second pass
	m_oRenderer.SetModelMatrix(m_oWorldMatrix);
	m_oRenderer.SetCurrentFBO(0);
	m_oRenderer.CullFace(0);
	UpdateRessource();
	m_oRenderer.CullFace(1);
}

void CMapEntity::RenderMap()
{
	m_oRenderer.CullFace(false);
	vector<CEntity*> entities;
	CollectMapEntities(entities);
	DisplayEntities(entities);
	m_oRenderer.CullFace(true);
}

void CMapEntity::CollectMapEntities(vector<CEntity*>& entities)
{
	for (int i = 0; i < m_pScene->GetChildCount(); i++) {
		CEntity* pEntity = dynamic_cast<CEntity*>(m_pScene->GetChild(i));
		if (pEntity) {
			if (pEntity != this) {
				CMobileEntity* pMobile = dynamic_cast<CMobileEntity*>(pEntity);
				if (!pMobile) {
					CLightEntity* pLightEntity = dynamic_cast<CLightEntity*>(pEntity);
					if (!pLightEntity)
						entities.push_back(pEntity);
				}
			}
		}
	}
}

void CMapEntity::DisplayEntities(vector<CEntity*>& entities)
{
	ICamera* pActiveCamera = m_oCameraManager.GetActiveCamera();
	CMatrix oCamMatrix;
	m_pMapCamera->Update();
	m_pMapCamera->GetWorldMatrix(oCamMatrix);
	CMatrix oBackupInvCameraMatrix;
	m_oRenderer.GetInvCameraMatrix(oBackupInvCameraMatrix);
	m_oRenderer.SetCameraMatrix(oCamMatrix);
	m_oCameraManager.SetActiveCamera(m_pMapCamera);
	IShader* pBackupShader = NULL;
	IShader* pFirstPassShader = m_oRenderer.GetShader(m_sFirstPassShaderName);

	CMatrix m;
	m_oRenderer.SetModelMatrix(m);
	IMesh* pGround = static_cast<IMesh*>(m_pScene->GetRessource());
	pBackupShader = pGround->GetShader();
	pGround->SetShader(pFirstPassShader);
	pGround->Update();
	pGround->SetShader(pBackupShader);

	for (int i = 0; i < entities.size(); i++) {
		pBackupShader = entities[i]->GetRessource()->GetShader();
		entities[i]->SetShader(pFirstPassShader);
		entities[i]->Update();
		entities[i]->SetShader(pBackupShader);
	}

	m_oCameraManager.SetActiveCamera(pActiveCamera);
	m_oRenderer.SetInvCameraMatrix(oBackupInvCameraMatrix);
}