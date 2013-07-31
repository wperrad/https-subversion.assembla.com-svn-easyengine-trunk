#ifndef CAMERA_TEST_H
#define CAMERA_TEST_H

#include "Camera.h"


class CLinkedCamera : public CCamera
{
public:
	CLinkedCamera( float fFov );
	virtual ~CLinkedCamera();
	void Move( float fOffsetYaw, float fOffsetPitch, float fOffsetRoll, float fAvanceOffet, float fLeftOffset, float fUpOffset );
};


#endif //CAMERA_TEST_H