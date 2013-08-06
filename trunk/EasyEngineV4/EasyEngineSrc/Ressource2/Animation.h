#ifndef ANIMATION_H
#define ANIMATION_H

// stl
#include <map>
#include <vector>


#include "IRessource.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "ISystems.h"

class ISystemsManager;

using namespace std;

//#define OLD_METHOD

class CAnimation : public IAnimation
{
	struct CCallback
	{
		TCallback	m_pCallback;
		void*		m_pCallbackData;
	};

	map< int, IBone* >				m_mBones;
	map< int, vector< CKey > >		m_mBoneKeys;
	IBone*							m_pSkeletonRoot;

	int								m_nLastTickCount;
	int								m_nCurrentTickCount;
	int								m_nCurrentAnimationTime;
	bool							m_bLoop;
	float							m_fSpeed;
	bool							m_bPause;
	bool							m_bStart;
	int								m_nStartAnimationTime;
	int								m_nEndAnimationTime;
	vector< CCallback >				m_vCallback;
	

	float							GetInterpolatedFactor( int nBoneID );
	void							UpdateAnimationTime();
	void							CallCallbacks( TEvent e );
	

public:
	CAnimation( const IRessource::Desc& oDesc );
	// TODO : Remplacer AddBone et AddKey par une initialisation dans la desc
	void			AddBone( int nBoneID, const ISystemsManager& oSystemManager  );
	void			AddKey( int nBoneID, int nTimeValue, CKey::TKey eKeyType, const CMatrix& oLocalTM, const CMatrix& oWorldTM, const CQuaternion& q  );
	void			SetSkeleton( IBone* pBone );

	void			Play( bool bLoop );
	void			Pause( bool bPause );
	void			Stop();
	void			SetSpeed( float fSpeed );
	void			Update();
	bool			GetPause();
	void			SetShader( IShader* pShader ){}
	IShader*		GetCurrentShader() const{ return NULL; }
	void			SetStartAnimationTime( int nTime );
	void			SetEndAnimationTime( int nTime );
	void			NextFrame();
	void			SetAnimationTime( int nTime );
	int				GetAnimationTime();
	void			AddCallback( TCallback pCallback, void* pData );
	void			RemoveCallback( TCallback pCallback );
	void			RemoveAllCallback();
	int				GetStartAnimationTime();
	int				GetEndAnimationTime();
	void			GetBoneKeysMap( map< int, vector< CKey > >& mBoneKeys );
};

#endif // ANIMATION_H