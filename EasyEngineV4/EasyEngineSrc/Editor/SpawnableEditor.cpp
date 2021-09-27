#include "SpawnableEditor.h"
#include "Interface.h"
#include "IInputManager.h"
#include "IRenderer.h"
#include "IEntity.h"
#include "ICameraManager.h"
#include "ICamera.h"
#include "IGeometry.h"
#include "ICollisionManager.h"
#include "IHud.h"
#include "IConsole.h"


CSpawnableEditor::CSpawnableEditor(EEInterface& oInterface) :
CEditor(oInterface),
m_oHud(*static_cast<IHud*>(oInterface.GetPlugin("HUD"))),
m_bEditionMode(false),
m_pCurrentAddedEntity(nullptr),
m_pScene(nullptr),
m_pSelectedEntity(nullptr),
m_bDisplayPickingRay(false),
m_nHudX(800),
m_nHudY(150),
m_nHudLineHeight(15)
{
	IEventDispatcher* pEventDispatcher = static_cast<IEventDispatcher*>(oInterface.GetPlugin("EventDispatcher"));
	pEventDispatcher->AbonneToMouseEvent(this, OnMouseEventCallback);
}

void CSpawnableEditor::HandleEditorManagerCreation(IEditorManager* pEditorManager)
{
	m_pEditorManager = pEditorManager;
}

void CSpawnableEditor::OnMouseEventCallback(CPlugin* plugin, IEventDispatcher::TMouseEvent e, int x, int y)
{
	CSpawnableEditor* pEditor = dynamic_cast<CSpawnableEditor*>(plugin);
	if (pEditor->m_bEditionMode) {
		if (e == IEventDispatcher::TMouseEvent::T_LBUTTONDOWN)
			pEditor->OnLeftMouseDown(x, y);
		else if (e == IEventDispatcher::TMouseEvent::T_RBUTTONDOWN)
			pEditor->m_oInputManager.SetEditionMode(false);
		else if (e == IEventDispatcher::TMouseEvent::T_RBUTTONUP) {
			pEditor->m_oInputManager.SetEditionMode(true);
		}
		else if (e == IEventDispatcher::TMouseEvent::T_MOVE) {
			if (pEditor->m_pCurrentAddedEntity) {
				CVector intersect;
				pEditor->GetRayPlanIntersection(x, y, pEditor->GetPlanHeight(), intersect);
				pEditor->m_pCurrentAddedEntity->SetLocalPosition(intersect.m_x, pEditor->GetPlanHeight(), intersect.m_z);
			}
		}
	}
}

void CSpawnableEditor::OnLeftMouseDown(int x, int y)
{
	if (m_pCurrentAddedEntity) {
		m_pCurrentAddedEntity->SetWeight(1.f);
		m_pScene->UpdateMapEntities();
		OnEntityAdded();
		m_pCurrentAddedEntity = NULL;
	}
	else {
		CVector intersect;
		SelectEntity(x, y);
		if (m_pSelectedEntity) {
			m_pCurrentAddedEntity = m_pSelectedEntity;
			CVector pos;
			m_pCurrentAddedEntity->GetWorldPosition(pos);
			m_pCurrentAddedEntity->SetLocalPosition(pos.m_x, GetPlanHeight(), pos.m_z);
			m_pCurrentAddedEntity->SetWeight(0.f);
			m_pCurrentAddedEntity->Update();

			OnEntitySelected();
		}
	}
}

void CSpawnableEditor::GetRayPlanIntersection(int x, int y, float h, CVector& intersect)
{
	CVector camPos, ray_wor;
	RayCast(x, y, camPos, ray_wor);

#ifdef DEBUG_TEST_PLANE
	IEntity* pLineEntity = m_oEntityManager.CreateLineEntity(camPos, ray_wor);
	pLineEntity->Link(m_pScene);
#endif // DEBUG_TEST_PLANE

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

#ifdef DEBUG_TEST_PLANE
	IEntity* pSphere = m_oEntityManager.CreateSphere(100.f);
	pSphere->Link(m_pScene);
	pSphere->SetLocalPosition(intersect);

	if (!m_pQuadEntity) {
		m_pQuadEntity = m_oEntityManager.CreateQuad(d.m_x, d.m_z);
		m_pQuadEntity->Link(m_pScene);
		m_pQuadEntity->SetLocalPosition(0.f, h, 0.f);
	}
#endif // DEBUG_TEST_PLANE
}

void CSpawnableEditor::RayCast(int x, int y, CVector& p1, CVector& ray)
{
	CMatrix P, Pinv;
	m_oRenderer.GetProjectionMatrix(P);
	P.GetInverse(Pinv);
	CMatrix Vinv;
	m_oCameraManager.GetActiveCamera()->GetWorldMatrix(Vinv);

	int width, height;
	m_oRenderer.GetResolution(width, height);
	float logicalx = (2.f * (float)x / (float)width) - 1.f;
	float logicaly = 1.f - (2.f * (float)y / (float)height);

	CVector ray_nds(logicalx, logicaly, 1.f, 1.f);
	CVector ray_clip = CVector(ray_nds.m_x, ray_nds.m_y, -1.0, 1.0);

	CVector ray_eye = Pinv * ray_clip;
	ray_eye = CVector(ray_eye.m_x, ray_eye.m_y, -1.f, 0.f);

	ray = Vinv * ray_eye;
	ray.Normalize();

	Vinv.GetPosition(p1);
}

void CSpawnableEditor::SelectEntity(int x, int y)
{
	CVector camPos, ray_wor;
	RayCast(x, y, camPos, ray_wor);
	CVector B = camPos + ray_wor * 50000.f;
	B.m_w = 1.f;

	if (m_bDisplayPickingRay) {
		IEntity* pLineEntity = m_oEntityManager.CreateLineEntity(camPos, B);
		pLineEntity->Link(m_pScene);
	}

	IEntity* pSelectedEntity = NULL;
	IScene* pScene = dynamic_cast<IScene*>(m_pScene);
	vector<IEntity*> entities;
	pScene->CollectMinimapEntities(entities);
	for (int i = 0; i < entities.size(); i++) {
		IEntity* pEntity = entities[i];
		CVector pos;
		pEntity->GetWorldPosition(pos);
		if (IsIntersect(camPos, B, pos, pEntity->GetBoundingSphereRadius())) {
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
		DisplayLocalRepere();
	}
	else if (m_pSelectedEntity) {
		m_pSelectedEntity->DrawBoundingBox(false);
	}
}

void CSpawnableEditor::DisplayLocalRepere()
{
	return;
	CVector position;
	m_pSelectedEntity->GetWorldPosition(position);
	IEntity* x = m_oEntityManager.CreateCylinder(500, 10000);
}

bool CSpawnableEditor::IsIntersect(const CVector& linePt1, const CVector& linePt2, const CVector& M, float radius) const
{
	CVector P12 = linePt2 - linePt1;
	CVector P1M = M - linePt1;
	float alpha = acosf((P12 * P1M) / (P12.Norm() * P1M.Norm()));
	float d = P1M.Norm() * sinf(alpha);
	return d < radius;
}

void CSpawnableEditor::SetEditionMode(bool bEditionMode)
{
	if (m_bEditionMode != bEditionMode) {
		m_bEditionMode = bEditionMode;
		m_pEditorManager->CloseAllEditor();
		m_bEditionMode = bEditionMode;
		m_oInputManager.SetEditionMode(m_bEditionMode);
		if (m_bEditionMode) {
			ICamera* pFreeCamera = m_oCameraManager.GetCameraFromType(ICameraManager::T_FREE_CAMERA);
			m_oCameraManager.SetActiveCamera(pFreeCamera);
		}
		else {
			ICamera* pLinkCamera = m_oCameraManager.GetCameraFromType(ICameraManager::T_LINKED_CAMERA);
			m_oCameraManager.SetActiveCamera(pLinkCamera);
		}

		m_oHud.Clear();
		if (m_bEditionMode) {
			m_oHud.Print("Mode Edition : Vous pouvez importer un modele en utilisant la commande 'SpawnEntity'", m_nHudX, m_nHudY);
			m_oHud.Print("Vous pouvez sauvegarder le niveau grace la commande 'SaveLavel(levelName)'", m_nHudX, m_nHudY + m_nHudLineHeight);
		}
	}
}


void CSpawnableEditor::DisplayPickingRay(bool enable)
{
	m_bDisplayPickingRay = enable;
}

bool CSpawnableEditor::IsEnabled()
{
	return m_bEditionMode;
}

void CSpawnableEditor::InitSpawnedEntity()
{
	m_pCurrentAddedEntity->Link(m_pScene);
	m_pCurrentAddedEntity->SetLocalPosition(0, GetPlanHeight(), 0);
	m_pCurrentAddedEntity->SetWeight(0.f);
	m_pCurrentAddedEntity->Update();
}