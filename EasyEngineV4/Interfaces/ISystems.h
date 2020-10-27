#ifndef ISYSTEMS_H
#define ISYSTEMS_H

#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "Utils2/Node.h"
#include "EEPlugin.h"
#include <string>

using namespace std;

class IBox;
class ISphere;
class IGeometryManager;


struct CKey
{
	enum TKey{ eRotKey = 0, ePosKey };
	CKey() : m_nTimeValue( 0 ){}
	int				m_nTimeValue;
	CMatrix			m_oWorldTM;
	CMatrix			m_oLocalTM;
	CQuaternion		m_oQuat;
	TKey			m_eType;
};


class IBone : public CNode
{
public:
	virtual void			AddKey(  string sAnimation, int nTimeValue, const CMatrix& m, const CQuaternion& q ) = 0;
	virtual void			AddKey(  string sAnimation, CKey& oKey ) = 0;
	virtual void			NextKey() = 0;
	virtual void			Rewind() = 0;
	virtual void			UpdateTime( float fTime ) = 0;
	virtual void			GetKeyByIndex( int nIndex, CKey& oKey ) const = 0;
	virtual void			GetKeyByTime( int nTime, CKey& oKey ) const = 0;
	virtual int				GetKeyCount() const = 0;
	virtual void			SetCurrentAnimation( string sAnimation ) = 0;
	virtual void			SetBoundingBox( IBox* oBox ) = 0;
	virtual const IBox*		GetBoundingBox() = 0;
	virtual const ISphere*	GetBoundingSphere() = 0;
};

class ISystemsManager : public CPlugin
{
public:
	struct Desc : public CPlugin::Desc
	{
		IGeometryManager&	m_oGeometryManager;
		Desc( IGeometryManager& oGeometryManager ) : CPlugin::Desc( NULL, "" ), m_oGeometryManager( oGeometryManager ){}
	};

	ISystemsManager( Desc& oDesc ) : CPlugin( NULL, "SystemManager" ){}
	virtual IBone*	CreateBone() const = 0;

};

#endif // ISYSTEMS_H