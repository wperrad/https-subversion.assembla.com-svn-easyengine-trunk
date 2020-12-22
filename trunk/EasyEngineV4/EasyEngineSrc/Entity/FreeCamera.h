#ifndef FREECAMERA_H
#define FREECAMERA_H

#include "Camera.h"

class CFreeCamera : public CCamera
{
	float	m_fYaw;
	float	m_fPitch;
	string	m_sEntityName;
	float	m_fSpeed;

public:
	CFreeCamera( float fFov );
	void				Move( float fOffsetYaw, float fOffsetPitch, float fOffsetRoll, float fAvanceOffet, float fLeftOffset, float fUpOffset );
	void				Colorize(float r, float g, float b, float a) {}
	void				GetEntityName(string& sName);
	void				SetEntityName(string sName);
	void				Zoom(int value);
	float				GetSpeed();
	void				SetSpeed(float fSpeed);
	void				LocalTranslate(float x, float y, float z);
};


#endif // FREECAMERA_H