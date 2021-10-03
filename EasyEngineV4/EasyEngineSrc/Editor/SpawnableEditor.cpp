#include "SpawnableEditor.h"
#include "Interface.h"
#include "IInputManager.h"
#include "IRenderer.h"
#include "IEntity.h"
#include "ICamera.h"
#include "IGeometry.h"
#include "ICollisionManager.h"
#include "IConsole.h"
#include "EditorManager.h"

CSpawnableEditor::CSpawnableEditor(EEInterface& oInterface, ICameraManager::TCameraType cameraType) :
CEditor(oInterface, cameraType),
m_pCurrentAddedEntity(nullptr),
m_pScene(nullptr),
m_pSelectedEntity(nullptr),
m_bDisplayPickingRaySelected(false),
m_bDisplayPickingRayMouseMove(false),
m_bDisplayPickingIntersectPlane(false),
m_pQuadEntity(nullptr),
m_pDebugSphere(nullptr),
m_eLastKeyEvent(IEventDispatcher::TKeyEvent::T_KEYUP)
{
	IEventDispatcher* pEventDispatcher = static_cast<IEventDispatcher*>(oInterface.GetPlugin("EventDispatcher"));
	pEventDispatcher->AbonneToMouseEvent(this, OnMouseEventCallback);
	pEventDispatcher->AbonneToKeyEvent(this, OnKeyEventCallback);
}

void CSpawnableEditor::HandleEditorManagerCreation(IEditorManager* pEditorManager)
{
	m_pEditorManager = static_cast<CEditorManager*>(pEditorManager);
}

void CSpawnableEditor::OnMouseEventCallback(CPlugin* plugin, IEventDispatcher::TMouseEvent e, int x, int y)
{
	CSpawnableEditor* pEditor = dynamic_cast<CSpawnableEditor*>(plugin);
	if (pEditor->m_bEditionMode) {
		if (e == IEventDispatcher::TMouseEvent::T_LBUTTONDOWN)
			pEditor->OnLeftMouseDown(x, y);
		else if (e == IEventDispatcher::TMouseEvent::T_RBUTTONDOWN)
			pEditor->m_oInputManager.SetEditionMode(false);
		else if (e == IEventDispatcher::TMouseEvent::T_RBUTTONUP)
			pEditor->m_oInputManager.SetEditionMode(true);
		else if (e == IEventDispatcher::TMouseEvent::T_MOVE)
			pEditor->OnMouseMove(x, y);
	}
}

void CSpawnableEditor::OnKeyEventCallback(CPlugin* plugin, IEventDispatcher::TKeyEvent e, int key)
{
	if (e == IEventDispatcher::T_KEYDOWN) {
		CSpawnableEditor* pEditor = dynamic_cast<CSpawnableEditor*>(plugin);
		if (pEditor && pEditor->m_bEditionMode) {
			if (pEditor->m_pSelectedEntity) {
				if (key == VK_DELETE) {
					pEditor->OnEntityRemoved(pEditor->m_pSelectedEntity);
					pEditor->m_pSelectedEntity->Unlink();
					pEditor->m_pSelectedEntity = nullptr;
				}
			}
			else if (pEditor->m_pCurrentAddedEntity) {
				if (key == VK_DELETE) {
					pEditor->OnEntityRemoved(pEditor->m_pCurrentAddedEntity);
					pEditor->m_pCurrentAddedEntity->Unlink();
					pEditor->m_pCurrentAddedEntity = nullptr;
				}
				else if (key == VK_LEFT) {
					pEditor->m_pCurrentAddedEntity->Yaw(10.f);
				}
				else if (key == VK_RIGHT) {
					pEditor->m_pCurrentAddedEntity->Yaw(-10.f);
				}
			}
		}
		pEditor->m_eLastKeyEvent = e;
	}
}

void CSpawnableEditor::OnMouseMove(int x, int y)
{
	if (m_pCurrentAddedEntity) {
		CVector intersect;
		GetRayPlanIntersection(x, y, GetPlanHeight(), intersect);
		m_pCurrentAddedEntity->SetLocalPosition(intersect);
		if (m_bDisplayPickingRayMouseMove) {
			CVector camPos, ray_wor;
			RayCast(x, y, camPos, ray_wor);
			CVector farPoint = camPos + ray_wor * 50000.f;
			farPoint.m_w = 1.f;
			DisplayPickingRay(camPos, farPoint);
		}
	}
}

void CSpawnableEditor::OnLeftMouseDown(int x, int y)
{
	if (m_pCurrentAddedEntity) {
		m_pCurrentAddedEntity->SetWeight(1.f);
		m_pScene->UpdateMapEntities();
		OnEntityAdded();
		m_pCurrentAddedEntity->DrawBoundingBox(false);
		m_pCurrentAddedEntity = nullptr;
	}
	else {
		CVector intersect;
		SelectEntity(x, y);
		if (m_pSelectedEntity) {
			m_pCurrentAddedEntity = m_pSelectedEntity;
			CVector pos;
			m_pCurrentAddedEntity->GetWorldPosition(pos);
			m_pCurrentAddedEntity->Link(m_pScene);
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

	if (m_bDisplayPickingIntersectPlane) {
		if (!m_pDebugSphere) {
			m_pDebugSphere = m_oEntityManager.CreateSphere(50.f);
			m_pDebugSphere->Link(m_pScene);
		}
		m_pDebugSphere->SetLocalPosition(intersect);

		if (!m_pQuadEntity) {
			m_pQuadEntity = m_oEntityManager.CreateQuad(d.m_x, d.m_z);
			m_pQuadEntity->Link(m_pScene);
		}
		m_pQuadEntity->SetLocalPosition(0.f, h, 0.f);
	}
	else {
		if (m_pQuadEntity) {
			m_pQuadEntity->Unlink();
			delete m_pQuadEntity;
			m_pQuadEntity = nullptr;
		}
	}
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

void CSpawnableEditor::DisplayPickingRay(const CVector& camPos, const CVector& farPoint)
{
	IEntity* pLineEntity = m_oEntityManager.CreateLineEntity(camPos, farPoint);
	pLineEntity->Link(m_pScene);
}

void CSpawnableEditor::SelectEntity(int x, int y)
{
	CVector camPos, ray_wor;
	RayCast(x, y, camPos, ray_wor);
	CVector farPoint = camPos + ray_wor * 50000.f;
	farPoint.m_w = 1.f;

	if (m_bDisplayPickingRaySelected) {
		DisplayPickingRay(camPos, farPoint);
	}

	IEntity* pSelectedEntity = NULL;
	vector<IEntity*> entities;
	CollectSelectableEntity(entities);
	for (int i = 0; i < entities.size(); i++) {
		IEntity* pEntity = entities[i];
		CVector pos;
		pEntity->GetWorldPosition(pos);
		if (IsIntersect(camPos, farPoint, pos, pEntity->GetBoundingSphereRadius())) {
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


void CSpawnableEditor::EnableDisplayPickingRaySelected(bool enable)
{
	m_bDisplayPickingRaySelected = enable;
}

void CSpawnableEditor::EnableDisplayPickingRayMouseMove(bool enable)
{
	m_bDisplayPickingRayMouseMove = enable;
}

void CSpawnableEditor::EnableDisplayPickingIntersectPlane(bool enable)
{
	m_bDisplayPickingIntersectPlane = enable;
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

void CSpawnableEditor::InitCamera()
{
	m_oCameraManager.SetActiveCamera(m_pEditorCamera);
	IBox* pBBox = dynamic_cast<IBox*>(m_pScene->GetBoundingGeometry());
	if (pBBox) {
		CMatrix m;
		m_pEditorCamera->SetLocalMatrix(m);
		m_pEditorCamera->Move(0, -60.f, 0, 0, 0, pBBox->GetDimension().m_x / 8.f);
	}
}