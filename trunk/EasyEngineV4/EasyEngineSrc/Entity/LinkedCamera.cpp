#include "LinkedCamera.h"

CLinkedCamera::CLinkedCamera( float fFov ):
CCamera( fFov )
{
}

CLinkedCamera::~CLinkedCamera()
{
}

void CLinkedCamera::Move( float fOffsetYaw, float fOffsetPitch, float fOffsetRoll, float fAvanceOffet, float fLeftOffset, float fUpOffset )
{
	throw 1;
}