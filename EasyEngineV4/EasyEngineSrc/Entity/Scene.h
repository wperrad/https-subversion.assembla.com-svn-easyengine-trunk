#ifndef SCENE_NODE_H
#define SCENE_NODE_H

// stl
#include <vector>

// Engine
#include "Math/Matrix.h"
#include "EEPlugin.h"
#include "Entity.h"

class CNode;
class IRessourceManager;
class IRenderer;
class CFreeCamera;
class ICamera;
class IEntityManager;
class ICollisionManager;
class IPathFinder;
class CSphereEntity;

using namespace std;

class CScene : public CEntity, public IScene
{
public:
	struct Desc
	{
		IRessourceManager&	m_oRessourceManager;
		IRenderer&			m_oRenderer;
		std::string			m_sFileName;
		ICamera*			m_pCamera;
		ICameraManager&		m_oCameraManager;
		IEntityManager*		m_pEntityManager;
		ILoaderManager&		m_oLoaderManager;
		ICollisionManager&	m_oCollisionManager;
		IGeometryManager&	m_oGeometryManager;
		IPathFinder&		m_oPathFinder;
		Desc(	IRessourceManager& oRessourceManager, IRenderer& pRenderer, IEntityManager* pEntityManager, 
				ICamera* pCamera, ICameraManager& oCameraManager, ILoaderManager& oLoaderManager, 
				ICollisionManager& oCollisionManager, IGeometryManager& oGeometryManager, IPathFinder& oPathFinder);
	};

	enum TSceneMode
	{
		eNormalMode = 0,
		eMapMode
	};

	CScene( const Desc& desc );
	~CScene();
	
	IEntity*					Merge( string sRessourceName, string sEntityType, float x, float y, float z );
	IEntity*					Merge( string sRessourceName, string sEntityType, CMatrix& oXForm );
	void						Update();	
	void						Load( string sFileName );
	void						Export( string sFileName );
	void						Clear();
	float						GetGroundHeight( float x, float z );
	void						SetRessource( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, bool bDuplicate = false );
	IGrid*						GetCollisionGrid();
	void						CreateCollisionMap();
	void						RenderScene();
	void						RenderMap();
	ITexture*					CreateMapTexture();
	ITexture*					GetMapTexture();
	void						DisplayMap(bool display);


private:
	ICamera*					m_pCamera;
	ICameraManager&				m_oCameraManager;
	ILoaderManager&				m_oLoaderManager;
	ICollisionManager&			m_oCollisionManager;
	IPathFinder&				m_oPathFinder;
	int							m_nHeightMapID;
	string						m_sCollisionFileName;
	string						m_sHMFileName;
	bool						m_bCollisionMapCreated;
	bool						m_bHeightMapCreated;
	IGrid*						m_pCollisionGrid;
	ICamera*					m_pMapCamera;
	const string				m_sMapFirstPassShaderName;
	const string				m_sMapSecondPassShaderName;
	ITexture*					m_pMapTexture;
	vector<CEntity*>			m_vMapEntities;
	CEntity*					m_pPlayer;
	CEntity*					m_pPlayerMapSphere;
	bool						m_bDisplayMap;

	void						GetInfos(ILoader::CSceneInfos& si);
	void						Load(const ILoader::CSceneInfos& si);
	void						LoadSceneObject(const ILoader::CSceneObjInfos* pSceneObjInfos, IEntity* pParent);
	void						GetSkeletonEntities(CNode* pRoot, vector< IEntity* >& vEntity, string sFileFilter);
	ILoader::CSceneObjInfos*	GetEntityInfos(IEntity* pEntity);
	void						CreateCollisionGrid();
	void						CreateHeightMap();
	void						CollectMapEntities(vector<CEntity*>& entities);
	void						DisplayEntities(vector<CEntity*>& entities);
	void						OnChangeSector();
};

#endif // SCENE_NODE_H