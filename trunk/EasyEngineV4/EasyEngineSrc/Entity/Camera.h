#ifndef CAMERA_H
#define CAMERA_H

#include "ICamera.h"
#include "../Utils2/Node.h"


class CCamera : public ICamera
{
protected:
	float	m_fSpeed;
	bool	m_bFreeze;
	float	m_fFov;

public:
	CCamera( float fFov );
	float	GetSpeed();
	void	SetSpeed( float fSpeed );
	void	Freeze( bool bFreeze );
	void    LocalTranslate( float x, float y, float z );
	float	GetFov();
};

#endif // CAMERA_H