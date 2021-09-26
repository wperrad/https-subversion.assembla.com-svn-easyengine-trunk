#pragma once

#include "IEventDispatcher.h"
#include "ICameraManager.h"
#include "IEditor.h"

#include "../Utils/Math/Vector.h"

class IInputManager;
class ICollisionManager;
class IGeometryManager;
class IScene;
class IHud;
class CEditorManager;

class CSpawnableEditor : virtual public IEditor
{
public:
	CSpawnableEditor(EEInterface& oInterface);
	bool							IsEnabled() override;

protected:
	void							OnLeftMouseDown(int x, int y);
	void							GetRayPlanIntersection(int x, int y, float h, CVector& intersect);
	void							RayCast(int x, int y, CVector& p1, CVector& ray);
	void							SelectEntity(int x, int y);
	bool							IsIntersect(const CVector& linePt1, const CVector& linePt2, const CVector& M, float radius) const;
	void							DisplayLocalRepere();
	void							SetEditionMode(bool bEditionMode);
	void							HandleEditorManagerCreation(IEditorManager* pEditor) override;
	void							DisplayPickingRay(bool enable) override;
	virtual void					OnEntityAdded() = 0;
	virtual float					GetPlanHeight() = 0;
	virtual void					OnEntitySelected() = 0;
	virtual void					InitSpawnedEntity();
	

	static void						OnMouseEventCallback(CPlugin* plugin, IEventDispatcher::TMouseEvent, int x, int y);

	bool							m_bEditionMode;
	IEntity*						m_pCurrentAddedEntity;
	EEInterface&					m_oInterface;
	IInputManager&					m_oInputManager;
	IRenderer&						m_oRenderer;
	ICameraManager&					m_oCameraManager;
	ICollisionManager&				m_oCollisionManager;
	IGeometryManager&				m_oGeometryManager;
	IEntityManager&					m_oEntityManager;
	IEditorManager*					m_pEditorManager;
	IScene*							m_pScene;
	IEntity*						m_pSelectedEntity;
	bool							m_bDisplayPickingRay;
	int								m_nHudX;
	int								m_nHudY;
	IHud&							m_oHud;
	int								m_nHudLineHeight;
};