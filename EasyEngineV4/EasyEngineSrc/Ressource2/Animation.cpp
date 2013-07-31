#define ANIMATION_CPP

#include "Animation.h"
#include <windows.h> // GetTickCount()
#include "ISystems.h"

#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;




CAnimation::CAnimation( const IRessource::Desc& oDesc ):
IAnimation( oDesc ),
m_nCurrentAnimationTime( -1 ),
m_bLoop( true ),
m_fSpeed( 1.f ),
//m_nAnimationLenght( 16000 ),
m_nStartAnimationTime( 0 ),
m_nEndAnimationTime( 16000 ),
m_bPause( false ),
m_bStart( false ),
m_nLastTickCount( 0 ),
m_nCurrentTickCount( 0 ),
m_pSkeletonRoot( NULL )
{
}

void CAnimation::SetStartAnimationTime( int nTime )
{
	m_nStartAnimationTime = nTime;
}

void CAnimation::SetEndAnimationTime( int nTime )
{
	m_nEndAnimationTime = nTime;
}

void CAnimation::NextFrame()
{
	m_nCurrentAnimationTime += 160;
}

void CAnimation::AddBone( int nBoneID, const ISystemsManager& oSystemManager )
{
	vector< CKey > vKey;
	m_mBoneKeys.insert( map< int, vector< CKey > >::value_type( nBoneID, vKey ) );
}

void CAnimation::AddKey( int nBoneID, int nTimeValue, CKey::TKey eKeyType, const CMatrix& oLocalTM, const CMatrix& oWorldTM, const CQuaternion& q )
{
	CKey oKey;
	oKey.m_oLocalTM = oLocalTM;
	oKey.m_oWorldTM = oWorldTM;
	oKey.m_nTimeValue = nTimeValue;
	oKey.m_oQuat = q;
	oKey.m_eType = eKeyType;
	m_mBoneKeys[ nBoneID ].push_back( oKey );
}

void CAnimation::SetSkeleton( IBone* pBone )
{
	if( !m_pSkeletonRoot )
		m_pSkeletonRoot = pBone;
	map< int, vector< CKey > >::iterator itBone = m_mBoneKeys.find( pBone->GetID() );
	if( itBone != m_mBoneKeys.end() )
	{
		m_mBones[ pBone->GetID() ] = pBone;
		vector< CKey >& vKeys = m_mBoneKeys[ pBone->GetID() ];
		for( unsigned int iKey = 0; iKey < vKeys.size(); iKey++ )
			pBone->AddKey( m_sName, vKeys[ iKey ] );
	}
	for( unsigned int iBone = 0; iBone < pBone->GetChildCount(); iBone++ )
		SetSkeleton( static_cast< IBone* >( pBone->GetChild( iBone ) ) );
}

void CAnimation::UpdateAnimationTime()
{
	int nCurrentTickCount = GetTickCount();
	if ( m_nCurrentAnimationTime == -1 )
	{
		m_nCurrentAnimationTime = m_nStartAnimationTime;
		m_nLastTickCount = nCurrentTickCount;
	}
	if ( m_bPause || !m_bStart )
		m_nLastTickCount = GetTickCount();
	
	int nDeltaTickCount = nCurrentTickCount - m_nLastTickCount;
	m_nLastTickCount = nCurrentTickCount;
	m_nCurrentAnimationTime += (int)( (float)nDeltaTickCount * 4.f * m_fSpeed );
	if ( m_nCurrentAnimationTime > m_nEndAnimationTime )
	{
		CallCallbacks( eBeginRewind );
		for(  map< int, IBone* >::iterator itBoneKeys = m_mBones.begin(); itBoneKeys != m_mBones.end(); ++itBoneKeys )
			itBoneKeys->second->Rewind();
		m_nCurrentAnimationTime = m_nStartAnimationTime;
		if( !m_bLoop )
			Stop();
	}
}

void CAnimation::Play( bool bLoop )
{
	m_nCurrentAnimationTime = -1;
	m_nLastTickCount = GetTickCount();
	m_bStart = true;
	m_bPause = false;
	m_bLoop = bLoop;
	m_pSkeletonRoot->SetCurrentAnimation( m_sName );
	CallCallbacks( ePlay );
}

bool CAnimation::GetPause()
{
	return m_bPause;
}

void CAnimation::SetSpeed( float fSpeed )
{
	m_fSpeed = fSpeed;
}

void CAnimation::Pause( bool bPause )
{
	m_bPause = bPause;
}

void CAnimation::Update()
{
	UpdateAnimationTime();
	m_pSkeletonRoot->UpdateTime( (float)m_nCurrentAnimationTime );
	CallCallbacks( eAfterUpdate );
}

void CAnimation::SetAnimationTime( int nTime )
{
	m_nCurrentAnimationTime = nTime;
}

int CAnimation::GetAnimationTime()
{
	return m_nCurrentAnimationTime;
}

void CAnimation::Stop()
{
	m_nCurrentAnimationTime = m_nStartAnimationTime;
	for( map< int, IBone* >::iterator itBoneKeys = m_mBones.begin(); itBoneKeys != m_mBones.end(); ++itBoneKeys )
		itBoneKeys->second->Rewind();
	m_bPause = true;
}

void CAnimation::AddCallback( TCallback pCallback, void* pData )
{
	CCallback c;
	c.m_pCallback = pCallback;
	c.m_pCallbackData = pData;
	m_vCallback.push_back( c );
}

void CAnimation::CallCallbacks( TEvent e )
{
	for( unsigned int i = 0; i < m_vCallback.size(); i++ )
		if( m_vCallback[ i ].m_pCallback )
			m_vCallback[ i ].m_pCallback( e, m_vCallback[ i ].m_pCallbackData );
}

void CAnimation::RemoveCallback( TCallback pCallback )
{
	vector< CCallback >::iterator itCallback = m_vCallback.begin();
	for( itCallback; itCallback != m_vCallback.end(); itCallback++ )
	{
		if( itCallback->m_pCallback == pCallback )
		{
			m_vCallback.erase( itCallback );
			return;
		}
	}
}

int CAnimation::GetStartAnimationTime()
{
	return m_nStartAnimationTime;
}

int CAnimation::GetEndAnimationTime()
{
	return m_nEndAnimationTime;
}

void CAnimation::GetBoneKeysMap( map< int, vector< CKey > >& mBoneKeys )
{
	mBoneKeys = m_mBoneKeys;
}