#include "node.h"

using namespace std;

CNode::CNode() :
m_pParent( NULL ),
m_bQuaternionMode( false ),
m_nID( -1 ),
m_nParentID( -1 ),
m_bUpdateConstantLocalTranslate( false ),
m_bUpdateConstantLocalRotate( false )
{
}

CNode::~CNode()
{
	for ( unsigned int i = 0; i < m_vChild.size(); i++ )
		delete m_vChild[ i ];
}

CNode* CNode::GetParent()
{
	return m_pParent;
}

void CNode::SetQuaternionMode( bool bQuarternion )
{
	m_bQuaternionMode = bQuarternion;
	for ( unsigned int i = 0; i < m_vChild.size(); i++ )
		m_vChild.at( i )->SetQuaternionMode( bQuarternion );
}

void CNode::SetLocalQuaternion( const CQuaternion& oQuaternion )
{
	m_oLocalQuaternion = oQuaternion;
}

unsigned int CNode::GetChildCount() const
{
	return static_cast<unsigned int> ( m_vChild.size() );
}

CNode* CNode::GetChild( unsigned int nIdx ) const
{
	return m_vChild[ nIdx ];
}

void CNode::Update()
{
	if( m_bUpdateConstantLocalTranslate )
		LocalTranslate( m_vConstantLocalTranslate );
	if ( m_bQuaternionMode )
	{
		CQuaternion oParentWorldQuaternion;
		if ( m_pParent )
			m_pParent->GetWorldQuaternion( oParentWorldQuaternion );
		m_oWorldQuaternion = oParentWorldQuaternion * m_oLocalQuaternion;
	}
	else
	{
  		CMatrix oParentWorldMatrix;
  		if ( m_pParent )
  			m_pParent->GetWorldMatrix( oParentWorldMatrix );
  		m_oWorldMatrix = oParentWorldMatrix * m_oLocalMatrix;

	}
	for ( unsigned int i = 0; i < m_vChild.size(); i++ )
  	{
  	 	CNode* pNode = m_vChild[i];
  		pNode->Update();
  	}
	}

void CNode::UpdateWithoutChildren()
{
	CMatrix oParentWorldMatrix;
  	if ( m_pParent )
  		m_pParent->GetWorldMatrix( oParentWorldMatrix );
  	m_oWorldMatrix = oParentWorldMatrix * m_oLocalMatrix;
}

void CNode::Link( CNode* pNode )
{
	SetParent( pNode );
	pNode->AddChild(this);
}

void CNode::Unlink()
{
	for ( unsigned int i = 0; i< m_pParent->m_vChild.size(); i++ )
	{
		if( this == m_pParent->m_vChild[ i ] )
		{
			m_pParent->m_vChild[ i ] = m_pParent->m_vChild.back();
			m_pParent->m_vChild.resize( m_pParent->m_vChild.size() - 1 );
			break;
		}
	}
	m_pParent = NULL;
}

void CNode::AddChild( CNode* pNode )
{
	m_vChild.push_back(pNode);
}

void CNode::SetParent( CNode* pNode )
{
	if( m_pParent )
		Unlink();
	m_pParent = pNode;
	m_nParentID = m_pParent->GetID();
}

void CNode::LocalTranslate( float dx, float dy, float dz )
{
	m_oLocalMatrix = m_oLocalMatrix * CMatrix::GetTranslation( dx, dy, dz );
	//LocalTranslate( CVector( dx, dy, dz, 1 ) );
}

void CNode::LocalTranslate( const CVector& vTranslate )
{
	//m_oLocalMatrix = m_oLocalMatrix * CMatrix::GetTranslation( vTranslate );
	LocalTranslate( vTranslate.m_x, vTranslate.m_y, vTranslate.m_z );
}

bool bTest = false;

void CNode::WorldTranslate( float dx , float dy , float dz )
{
	//CMatrix localInv;
	//m_oLocalMatrix.GetInverse( localInv );
	//localInv.m_03 = 0.f;
	//localInv.m_13 = 0.f;
	//localInv.m_23 = 0.f;
	//CVector oLocalTranslation = localInv * CVector( dx, dy, dz, 1 );
	//if( bTest )
	//{
	//	m_oLocalMatrix.m_03 += dx;
	//	m_oLocalMatrix.m_13 += dy;
	//	m_oLocalMatrix.m_23 += dz;
	//}
	//else
	//	LocalTranslate( oLocalTranslation );
	WorldTranslate( CVector( dx, dy, dz, 1.f ) );
}

void CNode::WorldTranslate( const CVector& vTranslate )
{
	CMatrix localInv;
	m_oLocalMatrix.GetInverse( localInv );
	localInv.m_03 = 0.f;
	localInv.m_13 = 0.f;
	localInv.m_23 = 0.f;
	CVector oLocalTranslation = localInv * vTranslate;
	if( bTest )
	{
		m_oLocalMatrix.m_03 += vTranslate.m_x;
		m_oLocalMatrix.m_13 += vTranslate.m_y;
		m_oLocalMatrix.m_23 += vTranslate.m_z;
	}
	else
		LocalTranslate( oLocalTranslation );
}

void CNode::Yaw( float fAngle )
{
	m_oLocalMatrix = m_oLocalMatrix * CMatrix::GetyRotation( fAngle );
}

void CNode::Pitch( float fAngle )
{
	m_oLocalMatrix = m_oLocalMatrix * CMatrix::GetxRotation( fAngle );
}

void CNode::Roll(float fAngle)
{
	m_oLocalMatrix = m_oLocalMatrix * CMatrix::GetzRotation( fAngle );
}

void CNode::SetLocalPosition( float x, float y, float z )
{
  	m_oLocalMatrix.m_03 = x;
  	m_oLocalMatrix.m_13 = y;
  	m_oLocalMatrix.m_23 = z;
}

void CNode::SetWorldPosition( const CVector& vPos )
{
	CMatrix invParent;
	m_pParent->GetWorldMatrix().GetInverse(invParent);
	CVector localPos = invParent * vPos;
	m_oLocalMatrix.m_03 = localPos.m_x;
	m_oLocalMatrix.m_13 = localPos.m_y;
	m_oLocalMatrix.m_23 = localPos.m_z;
}

void CNode::SetWorldPosition(float x, float y, float z)
{
	SetWorldPosition(CVector(x, y, z));
}

void CNode::SetLocalPosition(const CVector& vPos)
{
	SetLocalPosition(vPos.m_x, vPos.m_y, vPos.m_z);
}

void CNode::GetWorldPosition( CVector& vPosition ) const
{
	vPosition.m_x = m_oWorldMatrix.m_03;
	vPosition.m_y = m_oWorldMatrix.m_13;
	vPosition.m_z = m_oWorldMatrix.m_23;
	vPosition.m_w = m_oWorldMatrix.m_33;
}

float CNode::GetX()
{
	return m_oWorldMatrix.m_03;
}

float CNode::GetY()
{
	return m_oWorldMatrix.m_13;
}

float CNode::GetZ()
{
	return m_oWorldMatrix.m_23;
}

void CNode::SetName( const string& sName )
{
	m_sName = sName;
}


void CNode::GetName( string& sName ) const
{
	sName = m_sName;
}

void CNode::SetLocalMatrix( const CMatrix& oMat )
{
	m_oLocalMatrix = oMat;
}

void CNode::SetWorldMatrix( const CMatrix& oMat )
{
	if (m_pParent) {
		CMatrix invParentWorld;
		m_pParent->GetWorldMatrix().GetInverse(invParentWorld);
		m_oLocalMatrix = invParentWorld * oMat;
	}
	else
		m_oWorldMatrix = oMat;
}

void CNode::GetLocalQuaternion( CQuaternion& oQuaternion )
{
	m_oLocalQuaternion = oQuaternion;
}

void CNode::GetLocalMatrix( CMatrix& oMat ) const
{
	oMat = m_oLocalMatrix;
}

const CMatrix& CNode::GetLocalMatrix() const
{
	return m_oLocalMatrix;
}

void CNode::ClearChilds()
{
	m_vChild.clear();
}

void CNode::UpdateTime( float fTime )
{
}

void CNode::ConstantLocalRotate( const CVector& vRotation )
{
	m_bUpdateConstantLocalRotate = true;
	m_vConstantLocalRotate = vRotation;
}

void CNode::ConstantLocalTranslate( const CVector& vTranslate )
{
	if( vTranslate == CVector( 0.f, 0.f, 0.f ) )
		m_bUpdateConstantLocalTranslate = false;
	else
		m_bUpdateConstantLocalTranslate = true;
	m_vConstantLocalTranslate = vTranslate;
}

float CNode::GetDistance( CNode* pNode )
{
	CVector oThisNodeCenter, oCenter;
	GetWorldPosition( oThisNodeCenter );
	pNode->GetWorldPosition( oCenter );
	return ( oThisNodeCenter - oCenter ).Norm();
}

CNode* CNode::DuplicateHierarchy()
{
	CNode* pNode = new CNode;
	*pNode = *this;
	pNode->m_vChild.clear();
	for ( unsigned int iNode = 0; iNode < GetChildCount(); iNode++ )
	{
		CNode* pChild = GetChild( iNode )->DuplicateHierarchy();
		pChild->Link( pNode );
	}
	return pNode;
}

void CNode::SetID( int nID )
{
	m_nID = nID;
}

int CNode::GetID() const
{
	return m_nID;
}

void CNode::GetWorldMatrix( CMatrix& oMatrix )
{
	oMatrix = m_oWorldMatrix;
}

const CMatrix& CNode::GetWorldMatrix() const
{
	return m_oWorldMatrix;
}

void CNode::GetWorldQuaternion( CQuaternion& oQuaternion )
{
	oQuaternion = m_oWorldQuaternion;
}

void CNode::GetHierarchyCount( int& nCount )
{
	nCount++;
	for ( unsigned int iNode = 0; iNode < GetChildCount(); iNode++ )
		GetChild( iNode )->GetHierarchyCount( nCount );
}

CNode* CNode::GetChildBoneByID( int nID )
{
	if( m_nID == nID )
		return this;
	else
	{
		CNode* pNodeRet = NULL;
		int i = 0;
		while( !pNodeRet && i < GetChildCount() )
		{
			pNodeRet = GetChild( i )->GetChildBoneByID( nID );
			i++;
		}
		return pNodeRet;
	}
}

CNode* CNode::GetChildBoneByName( string sName )
{
	if( m_sName == sName )
		return this;
	else
	{
		CNode* pNodeRet = NULL;
		int i = 0;
		while( !pNodeRet && i < GetChildCount() )
		{
			pNodeRet = GetChild( i )->GetChildBoneByName( sName );
			i++;
		}
		return pNodeRet;
	}
}

void CNode::SetLocalTMByWorldTM()
{
	if ( m_pParent )
	{
		CMatrix oParentWorldTM, oInverseParentWorldTM, oLocalTM;
		m_pParent->GetWorldMatrix( oParentWorldTM );
		oParentWorldTM.GetInverse( oInverseParentWorldTM );
		oLocalTM = oInverseParentWorldTM * m_oWorldMatrix;
		m_oLocalMatrix = oLocalTM;
	}
	else
		m_oLocalMatrix = m_oWorldMatrix;
	for( unsigned int i = 0; i < GetChildCount(); i++ )
		m_vChild[ i ]->SetLocalTMByWorldTM();
}