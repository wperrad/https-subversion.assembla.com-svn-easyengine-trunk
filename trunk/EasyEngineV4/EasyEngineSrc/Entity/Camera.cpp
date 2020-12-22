#include "Camera.h"

CCamera::CCamera( float fFov ):
m_bFreeze( false ),
m_fFov( fFov )
{
}




void CCamera::Freeze( bool bFreeze )
{
	m_bFreeze = bFreeze;
}

float CCamera::GetFov()
{
	return m_fFov;
}