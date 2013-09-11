#include "IGeometry.h"

class IBox;

class CGeometryManager : public IGeometryManager
{
	map< int, IBox* >		m_mBox;
	map< int, ISphere* >	m_mSphere;
public:
	CGeometryManager( CPlugin::Desc& oDesc );
	IWeightTable*	CreateWeightTable() const;
	IBox*			CreateBox();
	IBox*			CreateBox( const IBox& oBox );
	int				GetLastCreateBoxID();
	IBox*			GetBox( int nID ) const;
	ISphere*		CreateSphere();
	ISphere*		CreateSphere( CVector& oCenter, float fRadius );
	ISegment*		CreateSegment( const CVector& first, const CVector& last );
	ICylinder*		CreateCylinder( const CVector& oBase, float fRadius, float fHeight );
	ICircle*		CreateCircle( const CVector2D& oCenter, float fRadius );
	ISegment2D*		CreateSegment2D( const CVector2D& first, const CVector2D& last );
};

extern "C" _declspec(dllexport) IGeometryManager* CreateGeometryManager( IGeometryManager::Desc& oDesc );