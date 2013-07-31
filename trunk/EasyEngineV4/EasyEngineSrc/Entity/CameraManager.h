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
	//std::vector< ICamera* >		m_vCamera;
	map< TCameraType, ICamera* >	m_mCamera;
	map< ICamera*, TCameraType >	m_mCameraType;
	ICamera*					m_pActiveCamera;
	vector< IRenderer*	>		m_vRenderer;
	

public:
	CCameraManager( const ICameraManager::Desc& oDesc );
	~CCameraManager();
	ICamera*		CreateCamera( TCameraType, float fFov, IEntityManager& oEntityManager );
	void			SetActiveCamera( ICamera* pCamera );
	ICamera*		GetActiveCamera();
	void			MoveActive( float fOffsetYaw, float fOffsetPitch, float fOffsetRoll, float fAvanceOffet, float fLeftOffset, float fUpOffset );
	TCameraType		GetCameraType( ICamera* pCamera ) ;
	ICamera*		GetCameraFromType( TCameraType type );
};
extern "C" _declspec(dllexport) ICameraManager* CreateCameraManager( const ICameraManager::Desc& );

#endif // CAMERAMANAGER_H