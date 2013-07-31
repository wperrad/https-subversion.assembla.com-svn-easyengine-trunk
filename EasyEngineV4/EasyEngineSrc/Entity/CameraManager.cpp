#define CAMERAMANAGER_CPP

#include "CameraManager.h"
#include "FreeCamera.h"
#include "LinkedCamera.h"
#include "IRenderer.h"
#include "IEntity.h"

using namespace std;

CCameraManager::CCameraManager( const ICameraManager::Desc& oDesc ) : 
ICameraManager( oDesc ),
m_pActiveCamera( NULL )
{
	for ( unsigned int i = 0; i < oDesc.m_vRenderer.size(); i++ )
		m_vRenderer.push_back( oDesc.m_vRenderer[ i ] );
}

CCameraManager::~CCameraManager()
{
	map< TCameraType, ICamera* >::iterator itCam = m_mCamera.find( T_FREE_CAMERA );
	if( itCam != m_mCamera.end() )
		delete itCam->second;
}

ICamera* CCameraManager::CreateCamera( TCameraType type, float fFov, IEntityManager& oEntityManager )
{
	ICamera* pCamera = NULL;
	string sCameraName;
	switch( type )
	{
	case ICameraManager::T_FREE_CAMERA:
		pCamera = new CFreeCamera( fFov );
		sCameraName = "FreeCamera";
		break;
	case ICameraManager::T_LINKED_CAMERA:
		pCamera = new CLinkedCamera( fFov );
		sCameraName = "LinkedCamera";
		break;
	}
	m_mCameraType[ pCamera ] = type;
	m_mCamera[ type ] = pCamera;
	//oEntityManager.CreateEntity( sCameraName );
	oEntityManager.AddEntity( pCamera, sCameraName );
	return pCamera;
}

void CCameraManager::SetActiveCamera( ICamera* pCamera )
{
	m_pActiveCamera = pCamera;
	for ( unsigned int i= 0; i < m_vRenderer.size(); i++ )
		m_vRenderer.at( i )->SetFov( pCamera->GetFov() );
}

ICamera* CCameraManager::GetActiveCamera()
{
	return m_pActiveCamera;
}

ICameraManager::TCameraType	CCameraManager::GetCameraType( ICamera* pCamera ) 
{
	return m_mCameraType[ pCamera ];
}

ICamera* CCameraManager::GetCameraFromType( TCameraType type )
{
	return m_mCamera[ type ];
}

extern "C" _declspec(dllexport) ICameraManager* CreateCameraManager( const ICameraManager::Desc& oDesc )
{
	return new CCameraManager( oDesc );
}