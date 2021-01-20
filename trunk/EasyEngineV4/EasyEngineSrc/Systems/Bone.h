#include "ISystems.h"

using namespace std;

class IBox;

class CBone : public IBone
{
	IGeometryManager&						m_oGeometryManager;
	float									m_fLastFrameInterpolatedFactor;
	bool									m_bEnd;
	int										m_nLastKeyValue;
	int										m_nNextKeyValue;
	map< string, vector< CKey > > 			m_mKeys; // Associe � chaque animation son vecteur de cl�s correspondant
	vector<CKey>							m_vCurrentAnimationKeys;
	string									m_sCurrentAnimation;
	IBox*									m_pBoundingBox;
	ISphere*								m_pSphere;
	float									GetInterpolatedFactor( float fTime );
	void									GetInterpolatedMatrix( float fTime );
	void									GetRelativeInterpolatedMatrix( float fTime );
	void									GetRelativeInterpolatedMatrixSinceLastFrame( float fTime );
	void									GetInterpolatedQuaternion( float fTime, CQuaternion& q );
	bool									IsTimeToNextKey( float fTime );

public:
							CBone( IGeometryManager& oGeometryManager );
	
	void					AddKey(  string sAnimation, int nTimeValue, const CMatrix& m, const CQuaternion& q );
	void					AddKey(  string sAnimation, CKey& oKey );
	void					NextKey();
	void					Rewind();
	void					UpdateTime( float fTime );
	CNode*					DuplicateHierarchy();
	void					Yaw(float fAngle);
	void					Pitch(float fAngle);
  	void					Roll(float fAngle);
	void					GetKeyByIndex( int nIndex, CKey& oKey ) const;
	void					GetKeyByTime( int nTime, CKey& oKey ) const;
	int						GetKeyCount() const;
	void					SetCurrentAnimation( string sAnimation );
	void					SetBoundingBox( IBox* oBox );
	const IBox*				GetBoundingBox();
	const ISphere*			GetBoundingSphere();
	CBone&					operator=( CBone& oBone );
};

