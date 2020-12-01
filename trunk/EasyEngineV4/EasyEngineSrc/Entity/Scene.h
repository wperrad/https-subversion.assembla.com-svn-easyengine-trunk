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

using namespace std;

class CScene : public CEntity
{
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
	//CFightSystem				m_oFightSystem;

	void						GetInfos( ILoader::CSceneInfos& si );
	void						Load( const ILoader::CSceneInfos& si );
	void						LoadSceneObject( const ILoader::CSceneObjInfos* pSceneObjInfos, IEntity* pParent );
	void						GetSkeletonEntities( CNode* pRoot, vector< IEntity* >& vEntity, string sFileFilter );
	ILoader::CSceneObjInfos*	GetEntityInfos( IEntity* pEntity );
	void						CreateCollisionGrid();
	void						CreateHeightMap();

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

	CScene( const Desc& desc );
	~CScene();
	
	IEntity*				Merge( string sRessourceName, string sEntityType, float x, float y, float z );
	IEntity*				Merge( string sRessourceName, string sEntityType, CMatrix& oXForm );
	void					Update();
	
	void					Load( string sFileName );
	void					Export( string sFileName );
	void					Clear();
	float					GetHeight( float x, float z );
	void					SetRessource( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, bool bDuplicate = false );
	IGrid*					GetCollisionGrid();
	void					CreateCollisionMap();
	
	//CFightSystem&			GetFightSystem();
};

#endif // SCENE_NODE_H