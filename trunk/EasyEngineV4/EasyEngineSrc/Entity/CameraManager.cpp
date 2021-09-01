#define CAMERAMANAGER_CPP
#include "Interface.h"
#include "CameraManager.h"
#include "FreeCamera.h"
#include "LinkedCamera.h"
#include "IRenderer.h"
#include "IEntity.h"

using namespace std;

CCameraManager::CCameraManager(EEInterface& oInterface) :
m_oInterface(oInterface),
m_pActiveCamera( NULL )
{
	IRenderer* pRenderer = static_cast<IRenderer*>(oInterface.GetPlugin("Renderer"));
	m_vRenderer.push_back(pRenderer);
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
		pCamera = new CFreeCamera(m_oInterface, fFov);
		sCameraName = "FreeCamera";
		/*if (!m_pActiveCamera)
			m_pActiveCamera = pCamera;*/
		break;
	case ICameraManager::T_LINKED_CAMERA:
		pCamera = new CLinkedCamera(m_oInterface, fFov);
		sCameraName = "LinkedCamera";
		break;
	case ICameraManager::T_MAP_CAMERA:
		pCamera = new CFreeCamera(m_oInterface, fFov);
		sCameraName = "MapCamera";
		break;
	case ICameraManager::T_GUI_MAP_CAMERA:
		pCamera = new CFreeCamera(m_oInterface, fFov);
		sCameraName = "GuiMapCamera";
		break;
	default:
		pCamera = new CFreeCamera(m_oInterface, fFov);
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

string CCameraManager::GetName()
{
	return "CameraManager";
}

void CCameraManager::UnlinkCameras()
{
	for (map< TCameraType, ICamera* >::iterator it = m_mCamera.begin(); it != m_mCamera.end(); it++) {
		it->second->Unlink();
	}
	
}

extern "C" _declspec(dllexport) ICameraManager* CreateCameraManager(EEInterface& oInterface)
{
	return new CCameraManager(oInterface);
}