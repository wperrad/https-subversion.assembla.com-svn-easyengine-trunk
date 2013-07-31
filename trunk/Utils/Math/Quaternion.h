#ifndef QUATERNION_H
#define QUATERNION_H

// stl
#include <vector>

#include "Utils/IStorage.h"
// math
#include "Vector.h"

class CQuaternion : public IPersistantObject
{	

public:

	float m_x, m_y, m_z, m_w;
	CVector	m_vPosition;
	CQuaternion();
	CQuaternion( const CQuaternion& q );
	CQuaternion( float x, float y, float z, float w );
	CQuaternion( const CVector& vAxis, float fAngle );
	CQuaternion( const std::vector< float >& v );
	CQuaternion( const float* quat );
	void	GetMatrix( CMatrix& mat );
	float	GetAngle() const;
	void	GetAngleAxis( CVector& vAxis, float& fAngle ) const;
	void	GetConjugate( CQuaternion& oConjugate );
	float	GetNorm();
	void	Normalize();
	void	Fill( float x, float y, float z, float w );
	CQuaternion		operator+( const CQuaternion& q ) const;
	CQuaternion		operator*( float f ) const;
	CQuaternion		operator*( CQuaternion& q ) const;
	CQuaternion		operator/( float f ) const;

	CQuaternion		operator+=( const CQuaternion& q ) const;
	CQuaternion		operator*=( float f ) const;
	CQuaternion		operator/=( float f ) const;

	void Store( CBinaryFileStorage& store ) const;
	void Load( CBinaryFileStorage& store );
	void Store( CAsciiFileStorage& store ) const;
	void Load( CAsciiFileStorage& store );
	void Store( CStringStorage& store ) const;
	void Load( CStringStorage& store );

	static void Slerp( const CQuaternion& qInitial, const CQuaternion& qFinal, float t, CQuaternion& qResult );
	//static void	SlerpInvariantAxis( const CQuaternion& qInitial, const CQuaternion& qFinal, float t, CQuaternion& qResult );

};

#endif // QUATERNION_H