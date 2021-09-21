#include "SpawnableEditor.h"


void CSpawnableEditor::OnMouseEventCallback(CPlugin* plugin, IEventDispatcher::TMouseEvent e, int x, int y)
{
#if 0
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
				pEditor->GetRayPlanIntersection(x, y, pEditor->m_fPlanHeight, intersect);
				pEditor->m_pCurrentAddedEntity->SetLocalPosition(intersect.m_x, pEditor->m_fPlanHeight, intersect.m_z);
			}
		}
	}
#endif // 0
}