#include "IEntity.h"
#include "Scene.h"

// stl
#include <map>

class CSceneManager : public ISceneManager
{

	IRessourceManager&				m_oRessourceManager;
	IRenderer&						m_oRenderer;
	ICameraManager&					m_oCameraManager;
	ICollisionManager&				m_oCollisionManager;
	IEntityManager&					m_oEntityManager;
	ILoaderManager&					m_oLoaderManager;

	std::map< string, CScene* >		m_mStringScene;
	std::map< CScene*, string >		m_mSceneString;

	CScene*							GetScene( IEntity* pScene );

public:

	CSceneManager( const ISceneManager::Desc& oDesc );
	~CSceneManager();

	IEntity*	CreateScene( std::string sSceneName, std::string sMeshFileName, IGeometryManager& oGeometryManager );
	IEntity*	GetScene( std::string sSceneName );
	IEntity*	Merge( IEntity* pScene, string sRessourceName, string sEntityType, CMatrix& oXForm );
	IEntity*	Merge( IEntity* pScene, const std::string& sRessourceName, string sEntityType, float x = 0, float y = 0, float z = 0, bool bIsAnimated = false );
	void		Load( IEntity* pScene, string sFileName );
	void		Export( IEntity* pScene, string sFileName );
	void		ClearScene( IEntity* pScene );
	void		CreateCollisionMap(IEntity* pScene);
};

extern "C" _declspec(dllexport) ISceneManager* CreateSceneManager( const ISceneManager::Desc& oDesc );