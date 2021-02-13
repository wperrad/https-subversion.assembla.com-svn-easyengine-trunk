#include "IInputManager.h"
#include "Editor.h"
#include "ICameraManager.h"
#include "IRenderer.h"
#include "ICamera.h"
#include "IGeometry.h"
#include "Interface.h"
#include "IEntity.h"
#include "ICollisionManager.h"

extern IEntity* m_pScene;

CEditor::CEditor() :
CPlugin(NULL, "Editor"),
m_oInputManager(*static_cast<IInputManager*>(s_pEngineInterface->GetPlugin("InputManager"))),
m_oCameraManager(*static_cast<ICameraManager*>(s_pEngineInterface->GetPlugin("CameraManager"))),
m_oRenderer(*static_cast<IRenderer*>(s_pEngineInterface->GetPlugin("Renderer"))),
m_oEntityManager(*static_cast<IEntityManager*>(s_pEngineInterface->GetPlugin("EntityManager"))),
m_oGeometryManager(*static_cast<IGeometryManager*>(s_pEngineInterface->GetPlugin("GeometryManager"))),
m_oLoaderManager(*static_cast<ILoaderManager*>(s_pEngineInterface->GetPlugin("LoaderManager"))),
m_oRessourceManager(*static_cast<IRessourceManager*>(s_pEngineInterface->GetPlugin("RessourceManager"))),
m_oCollisionManager(*static_cast<ICollisionManager*>(s_pEngineInterface->GetPlugin("CollisionManager"))),
m_bEditionMode(false),
m_bAddEntityMode(false),
m_pCurrentAddedEntity(NULL),
m_fPlanHeight(3000.f),
m_pSelectedEntity(NULL),
m_fGroundAdaptationHeight(10.f)
{
	IEventDispatcher* pEventDispatcher = static_cast<IEventDispatcher*>(s_pEngineInterface->GetPlugin("EventDispatcher"));
	pEventDispatcher->AbonneToMouseEvent(this, OnMouseEventCallback);
	pEventDispatcher->AbonneToKeyEvent(this, OnKeyEventCallback);
	string sSceneFileName;
	m_pScene->SetLoadRessourceCallback(OnSceneLoadRessource, this);
}

void CEditor::SetEditionMode(bool bEditionMode)
{
	m_bEditionMode = bEditionMode;
	m_oInputManager.SetEditionMode(m_bEditionMode);
	if (bEditionMode) {
		ICamera* pFreeCamera = m_oCameraManager.GetCameraFromType(ICameraManager::T_FREE_CAMERA);
		m_oCameraManager.SetActiveCamera(pFreeCamera);
	}
	else {
		ICamera* pLinkCamera = m_oCameraManager.GetCameraFromType(ICameraManager::T_LINKED_CAMERA);
		m_oCameraManager.SetActiveCamera(pLinkCamera);
	}
}

void CEditor::OnMouseEventCallback(CPlugin* plugin, IEventDispatcher::TMouseEvent e, int x, int y)
{
	CEditor* pEditor = dynamic_cast<CEditor*>(plugin);
	if (pEditor->m_bEditionMode) {
		if (e == IEventDispatcher::TMouseEvent::T_LBUTTONDOWN) {
			if (pEditor->m_pCurrentAddedEntity) {
				pEditor->m_pCurrentAddedEntity->SetWeight(1.f);
				pEditor->m_pCurrentAddedEntity = NULL;
				IScene* pScene = dynamic_cast<IScene*>(m_pScene);
				pScene->UpdateMapEntities();
			}
			else {
				CVector intersect;
				pEditor->SelectEntity(x, y);
			}
		}
		else if (e == IEventDispatcher::TMouseEvent::T_RBUTTONDOWN)
			pEditor->m_oInputManager.SetEditionMode(false);
		else if (e == IEventDispatcher::TMouseEvent::T_RBUTTONUP) {
			pEditor->m_oInputManager.SetEditionMode(true);
		}
		else if (e == IEventDispatcher::TMouseEvent::T_MOVE) {
			if (pEditor->m_pCurrentAddedEntity) {
				CVector intersect;
				pEditor->GetRayPlanIntersection(x, y, pEditor->m_fPlanHeight, intersect);
				pEditor->m_pCurrentAddedEntity->SetLocalPosition(intersect.m_x, pEditor->m_fPlanHeight, intersect.m_z);
			}
		}
	}
}

void CEditor::OnKeyEventCallback(CPlugin* plugin, IEventDispatcher::TKeyEvent e, int n)
{
	if (e == IEventDispatcher::T_KEYUP) {
		CEditor* pEditor = static_cast<CEditor*>(plugin);
		if (pEditor->m_bEditionMode) 
		{
			if (n == VK_UP) {
				pEditor->ManageGroundAdaptation(pEditor->m_fGroundAdaptationHeight);
			}
			else if (n == VK_DOWN) {			
				pEditor->ManageGroundAdaptation(-pEditor->m_fGroundAdaptationHeight);
			}
		}
	}
}

void CEditor::ManageGroundAdaptation(float deltaHeight)
{
	m_fGroundAdaptationHeight = deltaHeight;
	AdaptGroundToEntity(m_pSelectedEntity);
	
}

void CEditor::RayCast(int x, int y, CVector& p1, CVector& ray)
{
	CMatrix P, Pinv;
	m_oRenderer.GetProjectionMatrix(P);
	P.GetInverse(Pinv);
	CMatrix V;
	m_oCameraManager.GetActiveCamera()->GetWorldMatrix(V);

	int width, height;
	m_oRenderer.GetResolution(width, height);
	float logicalx = (2.f * (float)x / (float)width) - 1.f;
	float logicaly = 1.f - (2.f * (float)y / (float)height);

	CVector ray_nds(logicalx, logicaly, 1.f, 1.f);
	CVector ray_clip = CVector(ray_nds.m_x, ray_nds.m_y, -1.0, 1.0);

	CVector ray_eye = Pinv * ray_clip;
	ray_eye = CVector(ray_eye.m_x, ray_eye.m_y, -1.f, 0.f);

	ray = V * ray_eye;
	ray.Normalize();

	V.GetPosition(p1);
}

bool CEditor::IsIntersect(const CVector& linePt1, const CVector& linePt2, const CVector& M, float radius) const
{
	CVector P12 = linePt2 - linePt1;
	CVector P1M = M - linePt1;
	float alpha = acosf( (P12 * P1M) / (P12.Norm() * P1M.Norm()) );
	float d = P1M.Norm() * sinf(alpha);
	return d < radius;
}

void CEditor::GetRayPlanIntersection(int x, int y, float h, CVector& intersect)
{
	CVector camPos, ray_wor;
	RayCast(x, y, camPos, ray_wor);

#ifdef DEBUG_TEST
	IEntity* pLineEntity = m_oEntityManager.CreateLineEntity(camPos, ray_wor);
	pLineEntity->Link(m_pScene);
#endif // DEBUG_TEST

	IMesh* pMesh = static_cast<IMesh*>(m_pScene->GetRessource());
	const CVector& d = pMesh->GetBBox()->GetDimension();
	IQuad* quad = m_oGeometryManager.CreateQuad(d.m_x, d.m_z);
	CMatrix quadTM;
	quadTM.SetPosition(0, h, 0);
	quad->SetTM(quadTM);

	CVector B = camPos + ray_wor * 50000.f;
	B.m_w = 1.f;
	quad->GetLineIntersection(camPos, B, intersect);
	delete quad;

#ifdef DEBUG_TEST
	IEntity* pSphere = m_oEntityManager.CreateSphere(100.f);
	pSphere->Link(m_pScene);
	pSphere->SetLocalPosition(intersect);

	if (!m_pQuadEntity) {
		m_pQuadEntity = m_oEntityManager.CreateQuad(d.m_x, d.m_z);
		m_pQuadEntity->Link(m_pScene);
		m_pQuadEntity->SetLocalPosition(0.f, h, 0.f);
	}
#endif // DEBUG_TEST
}

void CEditor::SelectEntity(int x, int y)
{
	CVector camPos, ray_wor;
	RayCast(x, y, camPos, ray_wor);
	ray_wor = ray_wor * 100000.f;
	ray_wor.m_w = 1.f;
	
	IEntity* pSelectedEntity = NULL;
	IScene* pScene = dynamic_cast<IScene*>(m_pScene);
	vector<IEntity*> entities;
	pScene->CollectMapEntities(entities);
	for (int i = 0; i < entities.size(); i++) {
		IEntity* pEntity = entities[i];		
		CVector pos;
		pEntity->GetWorldPosition(pos);
		if (IsIntersect(camPos, ray_wor, pos, pEntity->GetBoundingSphereRadius())) {
			float lastDistanceToCam = 999999999.f;
			CVector lastPos, currentPos;
			if (pSelectedEntity) {
				pSelectedEntity->GetWorldPosition(lastPos);
				lastDistanceToCam = (lastPos - camPos).Norm();
			}
			pEntity->GetWorldPosition(currentPos);
			float newDistanceToCam = (currentPos - camPos).Norm();
			if (newDistanceToCam < lastDistanceToCam) {
				pSelectedEntity = pEntity;
			}
		}
	}
	if (pSelectedEntity) {
		if (m_pSelectedEntity)
			m_pSelectedEntity->DrawBoundingBox(false);
		pSelectedEntity->DrawBoundingBox(true);
		m_pSelectedEntity = pSelectedEntity;
		//AdaptGroundToEntity(m_pLastSelectedEntity);
	}
}

void CEditor::AdaptGroundToEntity(IEntity* pEntity)
{
	IBox* pBox = dynamic_cast<IBox*>(pEntity->GetBoundingGeometry());
	if (pBox) {
		CVector dim = pBox->GetDimension();
		CVector pos;
		pEntity->GetWorldPosition(pos);
		ILoader::CMeshInfos& mi = m_oAnimatableMeshData.m_vMeshes[0];
		float hMin = 99999999.f;
		for (int i = 0; i < mi.m_vVertex.size() / 3; i++) {
			float x = mi.m_vVertex[i * 3];
			float y = mi.m_vVertex[i * 3 + 1];
			float z = mi.m_vVertex[i * 3 + 2];

			if ((x >= (pos.m_x - dim.m_x)) && (x <= (pos.m_x + dim.m_x)) &&
				(z >= (pos.m_z - dim.m_z)) && (z <= (pos.m_z + dim.m_z))) {
				if (hMin > y)
					hMin = y;
			}
		}
		hMin += m_fGroundAdaptationHeight;

		for (int i = 0; i < mi.m_vVertex.size() / 3; i++) {
			float x = mi.m_vVertex[i * 3];
			float z = mi.m_vVertex[i * 3 + 2];
			
			if ( (x >= (pos.m_x - dim.m_x)) && (x <= (pos.m_x + dim.m_x))  &&
				(z >= (pos.m_z - dim.m_z)) && (z <= (pos.m_z + dim.m_z))) {
				mi.m_vVertex[i * 3 + 1] = hMin;
			}
		}		
	}
}

void CEditor::SaveGround(string sFileName)
{
	m_oLoaderManager.Export(sFileName, m_oAnimatableMeshData);
	string name = sFileName.substr(0, sFileName.find('.'));
	m_oCollisionManager.CreateHeightMap(sFileName);
	IScene* pScene = dynamic_cast<IScene*>(m_pScene);
	string originalRessourceName;
	pScene->GetOriginalSceneFileName(originalRessourceName);
	pScene->SetOriginalSceneFileName(originalRessourceName);
	m_pScene->SetRessource(sFileName, m_oRessourceManager, m_oRenderer);	
	pScene->Export(name + ".bse");
}

void CEditor::SetGroundAdaptationHeight(float fHeight)
{
	m_fGroundAdaptationHeight = fHeight;
}

void CEditor::AddEntity(string sEntityFileName)
{
	m_bAddEntityMode = true;
	m_pCurrentAddedEntity = m_oEntityManager.CreateEntity(sEntityFileName, "", m_oRenderer);
	m_pCurrentAddedEntity->Link(m_pScene);
	m_pCurrentAddedEntity->SetLocalPosition(0, m_fPlanHeight, 0);
}

void CEditor::OnSceneLoadRessource(CPlugin* plugin)
{
	string sSceneFileName;
	m_pScene->GetRessource()->GetFileName(sSceneFileName);
	CEditor* pEditor = static_cast<CEditor*>(plugin);
	pEditor->m_oLoaderManager.Load(sSceneFileName, pEditor->m_oAnimatableMeshData);
}