#ifndef ICAMERA_H
#define ICAMERA_H

#define ENTITYCAMERA

#include "../Utils2/Node.h"

#ifdef ENTITYCAMERA
#include "IEntity.h"
#endif // ENTITYCAMERA

#pragma warning(disable:4250)



class ICamera : 
#ifdef ENTITYCAMERA
	public IEntity
#else
	public CNode
#endif // ENTITYCAMERA
{

public:
	virtual float	GetSpeed() = 0;
	virtual void	SetSpeed( float fSpeed ) = 0;
	virtual void	Freeze( bool bFreeze ) = 0;
	virtual void	Move( float fOffsetYaw, float fOffsetPitch, float fOffsetRoll, float fAvanceOffet, float fLeftOffset, float fUpOffset ) = 0;
	virtual float	GetFov() = 0;
	virtual void	Zoom(int value) = 0;
	virtual void	SetInventoryMode(bool bInventoryMode) = 0;
};

#endif // ICAMERA_H