#include "SceneManager.h"
#include "Scene.h"
#include "ICameraManager.h"

using namespace std;

CSceneManager::CSceneManager( const ISceneManager::Desc& oDesc ) :
ISceneManager( oDesc ),
m_oCameraManager( oDesc.m_oCameraManager ),
m_oRenderer( oDesc.m_oRenderer ),
m_oRessourceManager( oDesc.m_oRessourceManager ),
m_oEntityManager( oDesc.m_oEntityManager ),
m_oLoaderManager( oDesc.m_oLoaderManager ),
m_oCollisionManager( oDesc.m_oCollisionManager )
{
}

CSceneManager::~CSceneManager()
{
	for( map<string, CScene* >::iterator itScene = m_mStringScene.begin(); itScene != m_mStringScene.end(); ++itScene )
		delete itScene->second;
}

IEntity* CSceneManager::CreateScene( std::string sSceneName, std::string sMeshFileName, IGeometryManager& oGeometryManager )
{
	map< string, CScene* >::iterator itScene = m_mStringScene.find( sSceneName );
	if ( itScene == m_mStringScene.end() )
	{
		CScene::Desc oSceneDesc( m_oRessourceManager, m_oRenderer, &m_oEntityManager, m_oCameraManager.GetActiveCamera(), m_oCameraManager, m_oLoaderManager, m_oCollisionManager, oGeometryManager );
		oSceneDesc.m_sFileName = sMeshFileName;
		CScene* pScene = new CScene( oSceneDesc );
		m_mStringScene[ sSceneName ] = pScene;
		m_mSceneString[ pScene ] = sSceneName;
		m_oEntityManager.AddEntity( pScene, "SceneGame", 0 );
		return pScene;
	}
	return itScene->second;
}

IEntity* CSceneManager::GetScene( std::string sSceneName )
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

IEntity* CSceneManager::Merge( IEntity* pScene, string sRessourceName, string sEntityType, CMatrix& oXForm )
{
	CScene* pCastScene = GetScene( pScene );
	if( pCastScene )
		return pCastScene->Merge( sRessourceName, sEntityType, oXForm );
	CBadTypeException e( "CSceneManager::Merge() : pScene n'est pas une scene valide" );
	throw e;
}

IEntity* CSceneManager::Merge( IEntity* pScene, const std::string& sRessourceName, string sEntityType, float x, float y, float z, bool bIsAnimated )
{
	CMatrix oXForm;
	oXForm.m_03 = x;
	oXForm.m_13 = y;
	oXForm.m_23 = z;
	return Merge( pScene, sRessourceName, sEntityType, oXForm );
}

void CSceneManager::Load( IEntity* pScene, string sFileName )
{
	CScene* pCastScene = GetScene( pScene );
	if( pCastScene )
		pCastScene->Load( sFileName );
	else
	{
		CBadTypeException e( "CSceneManager::Load() : pScene n'est pas une scene valide" );
		throw e;
	}
}

void CSceneManager::Export( IEntity* pScene, string sFileName )
{
	CScene* pCastScene = GetScene( pScene );
	if( pCastScene )
		pCastScene->Export( sFileName );
	else
	{
		CBadTypeException e( "CSceneManager::Load() : pScene n'est pas une scene valide" );
		throw e;
	}
}

void CSceneManager::ClearScene( IEntity* pScene )
{
	CScene* pCastScene = GetScene( pScene );
	if( pCastScene )
		pCastScene->Clear();
}

void CSceneManager::CreateCollisionMap(IEntity* pScene)
{
	CScene* pCastScene = GetScene(pScene);
	if (pCastScene)
		pCastScene->CreateCollisionMap();
}

extern "C" _declspec(dllexport) ISceneManager* CreateSceneManager( const ISceneManager::Desc& oDesc )
{
	return new CSceneManager( oDesc );
}