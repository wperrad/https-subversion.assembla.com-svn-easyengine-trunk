#ifndef IGEOMETRY_H
#define IGEOMETRY_H

#include "EEPlugin.h"
#include <vector>
#include "Math/Vector.h"

using namespace std;


class IWeightTable
{
public:
	virtual void	Add( int iVertexIndex, int nBoneID, float fBoneWeight ) = 0;
	virtual bool	Get( int iVertexIndex, std::map< int, float >& mWeight ) const = 0;
	virtual int		GetVertexCount() const = 0;
	virtual void	GetArrays( vector<float >& vWeightVertex, vector< float >& vWeigtedVertexID ) = 0;
	virtual void	BuildFromArrays( const vector<float >& vWeightVertex, const vector< float >& vWeightedVertexID ) = 0;
};

class ISphere
{
public:
	virtual float				GetRadius() const = 0;
	virtual void				SetRadius( float fRadius ) = 0;
	virtual void				SetCenter( CVector& oCenter ) = 0;
	virtual const CVector&		GetCenter() const = 0;
	
};

class IBox : public IPersistantObject
{
public:

	enum TAxis
	{
		eAxisX = 0,
		eAxisY,
		eAxisZ
	};

	virtual const CVector&		GetMinPoint() const = 0;
	virtual void				GetWorldMatrix( CMatrix& m ) const = 0;
	virtual const CMatrix&		GetWorldMatrix() const = 0;
	virtual const CVector&		GetDimension() const = 0;
	virtual float				ComputeBoundingSphereRadius()const = 0;
	virtual float				ComputeBoundingCylinderRadius( TAxis eGeneratorAxis ) const = 0;
	virtual float				GetBoundingSphereRadius() const = 0;
	virtual void				AddPoint( const CVector& p ) = 0;
	virtual IBox&				operator=( const IBox& oBox ) = 0;
	virtual void				Set( const CVector& oMinPoint, const CVector& oDimension ) = 0;
	virtual void				SetWorldMatrix( const CMatrix& oMatrix ) = 0;
	virtual void				GetPoints( vector< CVector >& vPoints ) = 0;
	virtual void				GetCenter( CVector& oCenter ) const = 0;
};

class ISegment
{
public:
	virtual ~ISegment() = 0{}
	virtual void	ComputeProjectedPointOnLine( const CVector& oPointToProject, CVector& oProjectedPoint ) const = 0;
	virtual float	ComputeDistanceToPoint( const CVector& oPoint ) = 0;
	virtual void	Compute2DLineEquation( float& a, float& b, float& c ) const = 0;
	virtual void	GetPoints( CVector& p1, CVector& p2 ) const = 0;
	//virtual void	SetPoints( const CVector& first, const CVector last ) = 0;
};

class ICylinder
{
public:
	virtual CVector&	GetBase() = 0;
	virtual float		GetRadius() = 0;
	virtual float		GetHeight() = 0;
	virtual void		Set( const CVector& oBase, float fRadius, float fHeight ) = 0;
	virtual void		ComputeTangent( const CVector& oLinePoint, CVector& oTangentPoint, bool bLeft ) = 0;
	virtual bool		IsPointIntoCylinder( const CVector& oPoint ) const = 0;
};

class ICircle
{
public:
	virtual void		Set( const CVector2D& oCenter, float fRadius ) = 0;
	virtual bool		IsSegmentAtLeftSide( const CVector2D& oStartPoint, const CVector2D& oEndPoint ) const = 0;
	virtual bool		IsPointIntoCircle( const CVector2D& oPoint ) const = 0;
	virtual void		ComputeTangent( const CVector2D& oLinePoint, CVector2D& oTangentPoint, bool bLeft ) const = 0;
};

class IGeometryManager : public CPlugin
{
public:
	IGeometryManager() : CPlugin( NULL, "GeometryManager" ){}
	virtual IWeightTable*	CreateWeightTable() const = 0;
	virtual IBox*			CreateBox() = 0;
	virtual IBox*			CreateBox( const IBox& oBox ) = 0;
	virtual ISphere*		CreateSphere() = 0;
	virtual ISphere*		CreateSphere( CVector& oCenter, float fRadius ) = 0;
	virtual ISegment*		CreateSegment( const CVector& first, const CVector& last ) = 0;
	virtual ICylinder*		CreateCylinder( const CVector& oBase, float fRadius, float fHeight ) = 0;
	virtual ICircle*		CreateCircle( const CVector2D& oCenter, float fRadius ) = 0;
	virtual int				GetLastCreateBoxID() = 0;
	virtual IBox*			GetBox( int nID ) const = 0;
};

#endif // IGEOMETRY_H