#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <vector>
#include <map>
#include "ICameraManager.h"


class CNode;
class IRenderer;

using namespace std;

class CCameraManager : public ICameraManager
{
	map< TCameraType, ICamera* >	m_mCamera;
	map< ICamera*, TCameraType >	m_mCameraType;
	ICamera*					m_pActiveCamera;
	vector< IRenderer*	>		m_vRenderer;
	EEInterface&				m_oInterface;
	

public:
	CCameraManager(EEInterface& oInterface);
	~CCameraManager();
	ICamera*		CreateCamera( TCameraType, float fFov, IEntityManager& oEntityManager );
	void			SetActiveCamera( ICamera* pCamera );
	ICamera*		GetActiveCamera();
	TCameraType		GetCameraType( ICamera* pCamera ) ;
	ICamera*		GetCameraFromType( TCameraType type );
	string			GetName() override;
	void			UnlinkCameras() override;
};
extern "C" _declspec(dllexport) ICameraManager* CreateCameraManager(EEInterface& oInterface);

#endif // CAMERAMANAGER_H