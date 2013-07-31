#include ".\camera.h"

using namespace std;

CCamera::CCamera(void):
m_fSpeed(1),
m_bFreeze(false)
{
}

CCamera::~CCamera(void)
{
}


void CCamera::Translate(float dx , float dy , float dz)
{
	CVector vOffset =  m_matRotation * CVector(dx,dy,dz,1) ;		
	m_matTranslation.m_a14 -= vOffset.m_x;
	m_matTranslation.m_a24 -= vOffset.m_y;
	m_matTranslation.m_a34 += vOffset.m_z;
}

CMatrix CCamera::GetXFormTM()
{
	return m_matRotation *  m_matTranslation;
}

void CCamera::Update(float Yaw, float Pitch, float fAdvance, float fLeft)
{
	if (!m_bFreeze)
	{
		m_matRotation =  CMatrix::GetxRotation(Pitch) *CMatrix::GetyRotation(Yaw);				
		Yaw = 180.f - Yaw;
		CMatrix matRot2 = CMatrix::GetyRotation(Yaw) * CMatrix::GetxRotation(Pitch);			
		CVector v = matRot2 * CVector(fLeft, 0 , fAdvance, 1)*m_fSpeed;		
		m_matTranslation.m_a14 -= v.m_x;
		m_matTranslation.m_a24 -= v.m_y;
		m_matTranslation.m_a34 -= v.m_z;	
	}	
}


void CCamera::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;
}


float CCamera::GetSpeed()
{
	return m_fSpeed;
}

CVector CCamera::GetWorldPosition()
{
	return CVector(-m_matTranslation.m_a14 , -m_matTranslation.m_a24, -m_matTranslation.m_a34,-m_matTranslation.m_a44);
}


void CCamera::Freeze(bool bFreeze)
{
	m_bFreeze = bFreeze;
}

void CCamera::SetName( const string& sName )
{
	m_sName = sName;
}

void CCamera::GetName( string& sName )
{
	sName = m_sName;
}