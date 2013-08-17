#ifndef BOX_H
#define BOX_H

#include "math/Vector.h"
#include "math/Matrix.h"
#include "IGeometry.h"

class CBox : public IBox
{
	bool		m_bInitialized;
	float		m_fBoundingSphereRadius;
	CVector		m_oMinPoint;
	CMatrix		m_oWorldMatrix;
	CVector		m_oDimension;
	

public:

	const CVector&		GetMinPoint() const;
	void				GetWorldMatrix( CMatrix& m ) const;
	const CMatrix&		GetWorldMatrix() const;
	const CVector&		GetDimension() const;

	CBox();
	CBox( CVector& oMinPoint, CVector& oDimension );
	CBox( const CBox& oBox );
	void			Set( const CVector& oMinPoint, const CVector& oDimension );
	void			GetCenter( CVector& oCenter ) const;
	void			AddPoint( const CVector& p );
	float			GetBoundingSphereRadius() const;
	float			ComputeBoundingSphereRadius()const;
	float			ComputeBoundingCylinderRadius( TAxis eGeneratorAxis ) const;
	void			Store( CBinaryFileStorage& store ) const;
	void			Load( CBinaryFileStorage& store );
	void 			Store( CAsciiFileStorage& store ) const;
	void 			Load( CAsciiFileStorage& store );
	void 			Store( CStringStorage& store ) const;
	void 			Load( CStringStorage& store );
	IBox&			operator=( const IBox& oBox );
	void			SetWorldMatrix( const CMatrix& oMatrix );
	void			GetPoints( vector< CVector >& vPoints );
};

#endif // BOX_H