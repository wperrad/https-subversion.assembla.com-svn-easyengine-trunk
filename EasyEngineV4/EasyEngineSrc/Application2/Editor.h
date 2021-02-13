#include "EEPlugin.h"
#include "Interface.h"
#include "ILoader.h"
#include <string>

class IInputManager;
class ICameraManager;
class IEventDispatcher;
class IRenderer;
class IEntityManager;
class IGeometryManager;
class IEntity;
class CVector;
class EEInterface;
class ILoaderManager;
class IRessourceManager;
class ICollisionManager;


using namespace std;

class CEditor : public CPlugin
{
public:

	CEditor();

	void							SetEditionMode(bool bEditionMode);
	void							AddEntity(string sEntityFileName);
	void							SaveGround(string fileName);
	void							SetGroundAdaptationHeight(float fHeight);


private:
	static void						OnMouseEventCallback(CPlugin* plugin, IEventDispatcher::TMouseEvent, int x, int y);
	static void						OnKeyEventCallback(CPlugin*, IEventDispatcher::TKeyEvent e, int n);
	static void						OnSceneLoadRessource(CPlugin* plugin);

	void							GetRayPlanIntersection(int x, int y, float h, CVector& intersect);
	void							SelectEntity(int x, int y);
	void							RayCast(int x, int y, CVector& p1, CVector& ray);
	bool							IsIntersect(const CVector& linePt1, const CVector& linePt2, const CVector& M, float radius) const;
	void							AdaptGroundToEntity(IEntity* pEntity);
	void							ManageGroundAdaptation(float deltaHeight);

	bool							m_bEditionMode;
	IInputManager&					m_oInputManager;
	ICameraManager&					m_oCameraManager;
	IRenderer&						m_oRenderer;
	IEntityManager&					m_oEntityManager;
	IGeometryManager&				m_oGeometryManager;
	ILoaderManager&					m_oLoaderManager;
	IRessourceManager&				m_oRessourceManager;
	ICollisionManager&				m_oCollisionManager;
	bool							m_bAddEntityMode;
	IEntity*						m_pCurrentAddedEntity;
	float							m_fPlanHeight;
	ILoader::CAnimatableMeshData	m_oAnimatableMeshData;
	IEntity*						m_pSelectedEntity;
	float							m_fGroundAdaptationHeight;

#ifdef DEBUG_TEST
	IEntity* m_pQuadEntity;
#endif // DEBUG_TEST
};