#ifndef FREECAMERA_H
#define FREECAMERA_H

#include "Camera.h"

class CFreeCamera : public CCamera
{
	float	m_fYaw;
	float	m_fPitch;

public:
	CFreeCamera( float fFov );
	void	Move( float fOffsetYaw, float fOffsetPitch, float fOffsetRoll, float fAvanceOffet, float fLeftOffset, float fUpOffset );
};


#endif // FREECAMERA_H