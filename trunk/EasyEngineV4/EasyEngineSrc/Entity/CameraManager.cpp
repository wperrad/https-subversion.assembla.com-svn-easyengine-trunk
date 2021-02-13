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
}

ICamera* CCameraManager::CreateCamera( TCameraType type, float fFov, IEntityManager& oEntityManager )
{
	ICamera* pCamera = NULL;
	string sCameraName;
	switch( type )
	{
	case ICameraManager::T_FREE_CAMERA:
		pCamera = new CFreeCamera( fFov, *m_vRenderer[0]);
		sCameraName = "FreeCamera";
		break;
	case ICameraManager::T_LINKED_CAMERA:
		pCamera = new CLinkedCamera( fFov, *m_vRenderer[0]);
		sCameraName = "LinkedCamera";
		break;
	case ICameraManager::T_MAP_CAMERA:
		pCamera = new CFreeCamera(fFov, *m_vRenderer[0]);
		sCameraName = "MapCamera";
		break;
	case ICameraManager::T_GUI_MAP_CAMERA:
		pCamera = new CFreeCamera(fFov, *m_vRenderer[0]);
		sCameraName = "GuiMapCamera";
		break;
	default:
		pCamera = new CFreeCamera(fFov, *m_vRenderer[0]);
		sCameraName = "noNameCamera";
		break;
	}
	m_mCameraType[ pCamera ] = type;
	m_mCamera[ type ] = pCamera;
	oEntityManager.AddEntity( pCamera, sCameraName );
	return pCamera;
}

void CCameraManager::SetActiveCamera( ICamera* pCamera )
{
	m_pActiveCamera = pCamera;
	for (unsigned int i = 0; i < m_vRenderer.size(); i++) {
		if(pCamera)
			m_vRenderer.at(i)->SetFov(pCamera->GetFov());
	}
}

ICamera* CCameraManager::GetActiveCamera()
{
	return m_pActiveCamera;
}

ICameraManager::TCameraType	CCameraManager::GetCameraType( ICamera* pCamera ) 
{
	map< ICamera*, TCameraType >::iterator itCamera = m_mCameraType.find(pCamera);
	if (itCamera != m_mCameraType.end())
		return itCamera->second;
	return T_NONE;
}

ICamera* CCameraManager::GetCameraFromType( TCameraType type )
{
	return m_mCamera[ type ];
}

extern "C" _declspec(dllexport) ICameraManager* CreateCameraManager( const ICameraManager::Desc& oDesc )
{
	return new CCameraManager( oDesc );
}