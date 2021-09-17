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

	CScene(EEInterface& oInterface, string ressourceFileName, string diffuseFileName);
	~CScene();
	
	IEntity*							Merge( string sRessourceName, float x, float y, float z ) override;
	IEntity*							Merge( string sRessourceName, CMatrix& oXForm );
	void								Update();	
	void								Load( string sFileName );
	void								Export( string sFileName );
	void								Clear();
	float								GetGroundHeight( float x, float z );
	void								SetRessource( string sFileName, bool bDuplicate = false );
	IGrid*								GetCollisionGrid();
	void								CreateCollisionMap();
	void								RenderScene();
	void								RenderMinimap();
	ITexture*							CreateMinimapTexture();
	ITexture*							GetMinimapTexture();
	void								DisplayMinimap(bool display);
	void								SetGroundMargin(float margin);
	float								GetGroundMargin();
	void								GetOriginalSceneFileName(string& sFileName);
	void								SetOriginalSceneFileName(string sFileName);
	void								SetDiffuseFileName(string diffuseFileName) override;
	int									GetCurrentHeightMapIndex() override;
	void								SetLength(int length) override;
	void								SetHeight(float height) override;
	void								SetHMFile(string sHMFile) override;
	void								DeleteTempDirectories() override;


private:
	ICameraManager&						m_oCameraManager;
	ILoaderManager&						m_oLoaderManager;
	ICollisionManager&					m_oCollisionManager;
	IRessourceManager&					m_oRessourceManager;
	IPathFinder&						m_oPathFinder;
	IFileSystem&						m_oFileSystem;
	int									m_nHeightMapID;
	string								m_sCollisionFileName;
	string								m_sHMFileName;
	bool								m_bCollisionMapCreated;
	bool								m_bHeightMapCreated;
	IGrid*								m_pCollisionGrid;
	ICamera*							m_pMapCamera;
	const string						m_sMapFirstPassShaderName;
	const string						m_sMapSecondPassShaderName;
	ITexture*							m_pMinimapTexture;
	vector<IEntity*>					m_vMapEntities;
	CEntity*							m_pPlayer;
	CEntity*							m_pPlayerMapSphere;
	bool								m_bDisplayMinimap;
	float								m_fGroundMargin;
	string								m_sOriginalSceneFileName;
	ITexture*							m_pHeightMaptexture;
	IShader*							m_pGroundShader;
	bool								m_bUseDisplacementMap;
	float								m_fDisplacementRatioHeightSize;
	float								m_fTiling;
	string								m_sDiffuseFileName;
	int									m_nMapLength;
	float								m_fMapHeight;

	void								GetInfos(ILoader::CSceneInfos& si);
	void								Load(const ILoader::CSceneInfos& si);
	void								LoadSceneObject(const ILoader::CObjectInfos* pSceneObjInfos, CEntity* pParent);
	ILoader::CObjectInfos*				GetEntityInfos(CEntity* pEntity);
	void								CreateCollisionGrid();
	void								CreateHeightMap();
	void								CollectMapEntities(vector<IEntity*>& entities);
	void								DisplayEntities(vector<IEntity*>& entities);
	void								OnChangeSector();
	void								UpdateMapEntities();
};

#endif // SCENE_NODE_H