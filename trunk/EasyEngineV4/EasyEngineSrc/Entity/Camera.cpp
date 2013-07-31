#include "Camera.h"

CCamera::CCamera( float fFov ):
m_fSpeed( 1.f ),
m_bFreeze( false ),
m_fFov( fFov )
{
}

float CCamera::GetSpeed()
{
	return m_fSpeed;
}

void CCamera::SetSpeed( float fSpeed )
{
	m_fSpeed = fSpeed;
}

void CCamera::LocalTranslate( float x, float y, float z )
{
	CNode::LocalTranslate( m_fSpeed * x, m_fSpeed * y, m_fSpeed * z );
}

void CCamera::Freeze( bool bFreeze )
{
	m_bFreeze = bFreeze;
}

float CCamera::GetFov()
{
	return m_fFov;
}