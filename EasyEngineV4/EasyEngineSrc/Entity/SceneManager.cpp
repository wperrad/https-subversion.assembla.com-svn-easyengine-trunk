#include "Interface.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ICameraManager.h"
#include "ICollisionManager.h"

using namespace std;

CSceneManager::CSceneManager(EEInterface& oInterface) :
m_oInterface(oInterface),
m_oCameraManager(static_cast<ICameraManager&>(*oInterface.GetPlugin("CameraManager"))),
m_oRenderer(static_cast<IRenderer&>(*oInterface.GetPlugin("Renderer"))),
m_oRessourceManager(static_cast<IRessourceManager&>(*oInterface.GetPlugin("RessourceManager"))),
m_oEntityManager(static_cast<IEntityManager&>(*oInterface.GetPlugin("EntityManager"))),
m_oLoaderManager(static_cast<ILoaderManager&>(*oInterface.GetPlugin("LoaderManager"))),
m_oCollisionManager(static_cast<ICollisionManager&>(*oInterface.GetPlugin("CollisionManager")))
{
}

CSceneManager::~CSceneManager()
{
	for( map<string, CScene* >::iterator itScene = m_mStringScene.begin(); itScene != m_mStringScene.end(); ++itScene )
		delete itScene->second;
}

IScene* CSceneManager::CreateScene(string sSceneName, string sRessourceFileName, string diffuseFileName)
{
	map< string, CScene* >::iterator itScene = m_mStringScene.find( sSceneName );
	if ( itScene == m_mStringScene.end() )
	{
		CScene* pScene = new CScene(m_oInterface, sRessourceFileName, diffuseFileName);
		m_mStringScene[ sSceneName ] = pScene;
		m_mSceneString[ pScene ] = sSceneName;
		m_oEntityManager.AddEntity( pScene, "SceneGame", 0 );
		return pScene;
	}
	return itScene->second;
}

IScene* CSceneManager::GetScene( std::string sSceneName )
{
	map< string, CScene* >::iterator itScene = m_mStringScene.find( sSceneName );
	if ( itScene == m_mStringScene.end() )
	{
		string sMessage = string( "Scene " ) + sSceneName + " introuvable";
		exception e( sMessage.c_str() );
		throw e;
	}
	return itScene->second;
}

CScene* CSceneManager::GetScene( IEntity* pScene )
{
	CScene* pCastScene = dynamic_cast< CScene* >( pScene );
	if( pCastScene )
	{
		map< CScene*, string >::iterator itScene = m_mSceneString.find( pCastScene );
		if( itScene != m_mSceneString.end() )
			return itScene->first;
	}
	return NULL;
}

string CSceneManager::GetName()
{
	return "SceneManager";
}

extern "C" _declspec(dllexport) ISceneManager* CreateSceneManager(EEInterface& oInterface)
{
	return new CSceneManager(oInterface);
}