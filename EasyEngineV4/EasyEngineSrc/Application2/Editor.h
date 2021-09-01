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
class IHud;
class IScene;
class IHeightMap;

//#define DEBUG_TEST_PLANE

using namespace std;

class CEditor : public CPlugin
{
public:

	CEditor();

	void							SetEditionMode(bool bEditionMode);
	void							SpawnEntity(string sEntityFileName);
	void							SaveLevel(string fileName);
	void							SetGroundAdaptationHeight(float fHeight);
	string							GetName() override;
	void							DisplayLocalRepere();
	void							DisplayPickingRay(bool enable);

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
	void							UpdateGround();
	bool							CreateLevelFolderIfNotExists(string levelName, string& levelFolder);
	void							OnLeftMouseDown(int x, int y);


	struct AdaptGroundThreadStruct
	{
		CEditor* editor;
		CVector pos;
		CVector dim;
		float adaptationHeight;
		int hudMsgIdx;
	};
	static DWORD WINAPI				AdaptGround(void* params);
	void							OnEndAdaptGround(int hudMsgIdx);

	bool							m_bEditionMode;
	IInputManager&					m_oInputManager;
	ICameraManager&					m_oCameraManager;
	IRenderer&						m_oRenderer;
	IEntityManager&					m_oEntityManager;
	IGeometryManager&				m_oGeometryManager;
	ILoaderManager&					m_oLoaderManager;
	IRessourceManager&				m_oRessourceManager;
	ICollisionManager&				m_oCollisionManager;
	IFileSystem&					m_oFileSystem;
	IHud&							m_oHud;
	bool							m_bAddEntityMode;
	IEntity*						m_pCurrentAddedEntity;
	float							m_fPlanHeight;
	ILoader::CAnimatableMeshData	m_oAnimatableMeshData;
	IEntity*						m_pSelectedEntity;
	float							m_fGroundAdaptationHeight;
	int								m_nHudX;
	int								m_nHudY;
	IScene*							m_pScene;
	IHeightMap*						m_pHeightMap;
	int								m_nHudLineHeight;
	string							m_sTmpAdaptedHeightMapFileName;
	bool							m_bDisplayPickingRay;

	static IEventDispatcher::TKeyEvent	m_eLastKeyEvent;

#ifdef DEBUG_TEST_PLANE
	IEntity* m_pQuadEntity;
#endif // DEBUG_TEST_PLANE
};