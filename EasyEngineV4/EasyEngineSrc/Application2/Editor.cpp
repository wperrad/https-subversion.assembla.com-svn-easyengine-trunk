#include "IInputManager.h"
#include "Editor.h"
#include "ICameraManager.h"
#include "IRenderer.h"
#include "ICamera.h"


extern IEntity* m_pScene;

CEditor::CEditor(
	IInputManager& oInputManager,
	ICameraManager& oCameraManager,
	IEventDispatcher& oEventDispatcher,
	IRenderer& oRenderer,
	IEntityManager& oEntityManager):
	CPlugin(NULL, "Editor"),
	m_oInputManager(oInputManager),
	m_oCameraManager(oCameraManager),
	m_oRenderer(oRenderer),
	m_oEntityManager(oEntityManager),
	m_bEditionMode(false)
{
	oEventDispatcher.AbonneToMouseEvent(this, OnMouseEventCallback);
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
		if (e == IEventDispatcher::TMouseEvent::T_LBUTTONDOWN)
			pEditor->RayCast(x, y);
		else if (e == IEventDispatcher::TMouseEvent::T_RBUTTONDOWN)
			pEditor->m_oInputManager.SetEditionMode(false);
		else if (e == IEventDispatcher::TMouseEvent::T_RBUTTONUP) {
			pEditor->m_oInputManager.SetEditionMode(true);
		}
	}
}

void CEditor::RayCast(int x, int y)
{
	CMatrix P, Pinv;
	m_oRenderer.GetProjectionMatrix(P);
	P.GetInverse(Pinv);
	CMatrix V;
	m_oCameraManager.GetActiveCamera()->GetWorldMatrix(V);

	int w, h;
	m_oRenderer.GetResolution(w, h);
	float logicalx = (2.f * (float)x / (float)w) - 1.f;
	float logicaly = 1.f - (2.f * (float)y / (float)h);

	CVector ray_nds(logicalx, logicaly, 1.f, 1.f);
	CVector ray_clip = CVector(ray_nds.m_x, ray_nds.m_y, -1.0, 1.0);

	CVector ray_eye = Pinv * ray_clip;
	ray_eye = CVector(ray_eye.m_x, ray_eye.m_y, -1.f, 0.f);

	CVector ray_wor = V * ray_eye;
	ray_wor.Normalize();

	CVector camPos;
	V.GetPosition(camPos);
	IEntity* pLineEntity = m_oEntityManager.CreateLineEntity(camPos, ray_wor);
	pLineEntity->Link(m_pScene);
}