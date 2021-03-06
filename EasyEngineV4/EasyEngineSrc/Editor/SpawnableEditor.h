#pragma once

#include "IEventDispatcher.h"
#include "IEditor.h"
#include "Editor.h"

#include "../Utils/Math/Vector.h"

class IInputManager;
class ICollisionManager;
class IGeometryManager;
class IScene;
class CEditorManager;
class IConsole;

class CSpawnableEditor : virtual public ISpawnableEditor, public CEditor
{
public:
	CSpawnableEditor(EEInterface& oInterface, ICameraManager::TCameraType cameraType);
	bool							IsEnabled() override;

protected:
	enum Type
	{
		eNone = 0,
		eAdding,
		eEditing
	};

	void							OnLeftMouseDown(int x, int y);
	void							GetRayPlanIntersection(int x, int y, float h, CVector& intersect);
	void							RayCast(int x, int y, CVector& p1, CVector& ray);
	void							SelectEntity(int x, int y);
	bool							IsIntersect(const CVector& linePt1, const CVector& linePt2, const CVector& M, float radius) const;
	void							DisplayLocalRepere();
	void							HandleEditorManagerCreation(IEditorManager* pEditor) override;
	void							EnableDisplayPickingRaySelected(bool enable) override;
	void							EnableDisplayPickingRayMouseMove(bool enable) override;
	void							EnableDisplayPickingIntersectPlane (bool enable) override;
	void							SetEditionMode(bool bEnable) override;
	void							DisplayPickingRay(const CVector& camPos, const CVector& far);
	void							InitCamera();
	void							OnMouseMove(int x, int y);
	virtual void					OnEntityAdded() = 0;
	virtual float					GetPlanHeight() = 0;
	virtual void					OnEntitySelected() = 0;
	virtual void					OnEntityRemoved(IEntity* pEntity) = 0;
	virtual void					CollectSelectableEntity(vector<IEntity*>& entities) = 0;
	virtual void					InitSpawnedEntity();
	static void						OnMouseEventCallback(CPlugin* plugin, IEventDispatcher::TMouseEvent, int x, int y);
	static void						OnKeyEventCallback(CPlugin*, IEventDispatcher::TKeyEvent e, int n);	
	
	IEventDispatcher&				m_oEventDispatcher;
	IEntity*						m_pEditingEntity;
	IScene*							m_pScene;
	bool							m_bDisplayPickingRaySelected;
	bool							m_bDisplayPickingRayMouseMove;
	bool							m_bDisplayPickingIntersectPlane;
	IEntity*						m_pQuadEntity;
	IEntity*						m_pDebugSphere;
	IEventDispatcher::TKeyEvent		m_eLastKeyEvent;
	Type							m_eEditorMode;
};