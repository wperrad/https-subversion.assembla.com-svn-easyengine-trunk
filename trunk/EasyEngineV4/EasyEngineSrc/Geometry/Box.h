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
	CMatrix		m_oTM;
	CVector		m_oDimension;

	bool		TestBoxesCollisionIntoFirstBoxBase(const IBox& b1, const IBox& b2);
	float		GetDistanceInBase(const IBox& oBox) const;
	float		GetMinx(const vector< CVector >& vPoints) const;
	float		GetMiny(const vector< CVector >& vPoints) const;
	float		GetMinz(const vector< CVector >& vPoints) const;
	float		GetMaxx(const vector< CVector >& vPoints) const;
	float		GetMaxy(const vector< CVector >& vPoints) const;
	float		GetMaxz(const vector< CVector >& vPoints) const;
	

public:

	const CVector&		GetMinPoint() const;
	void				GetTM( CMatrix& m ) const;
	const CMatrix&		GetTM() const;
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
	void			GetBase(CVector& oBase);
	IGeometry*		Duplicate();
	float			GetHeight() const;
	void			Transform(const CMatrix& tm);
	float			GetDistance(const IGeometry& oGeometry) const;
	float			GetDistance(const CBox& oBox) const;
	float			GetDistance(const ICylinder& oBox) const;
	IBox&			operator=(const IBox& oBox);
	void			SetWorldMatrix(const CMatrix& oMatrix);
	void			GetPoints(vector< CVector >& vPoints);
	bool			IsIntersect(const IBox& box);

	const IPersistantObject& operator >> (CBinaryFileStorage& store) const;
	IPersistantObject& operator << (CBinaryFileStorage& store);
	const IPersistantObject& operator >> (CAsciiFileStorage& store) const;
	IPersistantObject& operator << (CAsciiFileStorage& store);
	const IPersistantObject& operator >> (CStringStorage& store) const;
	IPersistantObject& operator << (CStringStorage& store);
};

#endif // BOX_H