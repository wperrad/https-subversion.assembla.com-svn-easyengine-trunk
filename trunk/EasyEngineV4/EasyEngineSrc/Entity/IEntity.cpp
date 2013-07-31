#include "IEntity.h"
#include "TimeManager.h"

float m_fGravity = 1000.f;

float CBody::s_fEpsilonError = 0.0001f * m_fGravity * m_fGravity;
float CBody::s_fZCollisionError = 10.f;

float GetGravity()
{
	return m_fGravity;
}

void SetGravity( float fGravity )
{
	
}

CBody::CBody() :
m_fWeight( 0 )
{
}

void CBody::Update()
{
	unsigned long nTimeElapsedSinceLastUpdate = CTimeManager::Instance()->GetTimeElapsedSinceLastUpdate();
	if( m_fWeight > 0 )
		m_oSpeed.m_y -= (float)( nTimeElapsedSinceLastUpdate ) * GetGravity() / 1000.f;
	else
		m_oSpeed.m_y = 0.f;
}

void CBody::SetZCollisionError( float e )
{
	s_fZCollisionError = e;
}

float CBody::GetZCollisionError()
{
	return s_fZCollisionError;
}