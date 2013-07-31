#include "node.h"

using namespace std;

CNode::CNode() :
m_pParent( NULL )
{
	m_sName = "Noname";
}

CNode::~CNode()
{
	for ( unsigned int i = 0; i < m_vChild.size(); i++ )
		delete m_vChild[ i ];
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
  	CMatrix oParentMatrix;
  	if ( m_pParent )
  		oParentMatrix = m_pParent->m_oTransformMatrix;
  	m_oGlobalMatrix = oParentMatrix * m_oTransformMatrix;

  	for ( unsigned int i = 0; i < m_vChild.size(); i++ )
  	{
  	  	CNode* pNode = m_vChild[i];
  	  	pNode->Update();
  	}
}

void CNode::Link( CNode* pNode )
{
	SetParent(pNode);
	pNode->AddChild(this);
}

void CNode::AddChild( CNode* pNode )
{
	m_vChild.push_back(pNode);
}

void CNode::SetParent(CNode* pNode)
{
	m_pParent = pNode;
}

void CNode::LocalTranslate( float dx, float dy, float dz )
{
	m_oTransformMatrix = m_oTransformMatrix * CMatrix::GetTranslation( dx, dy, dz );
}

void CNode::Yaw( float fAngle )
{
	m_oTransformMatrix = m_oTransformMatrix * CMatrix::GetyRotation( fAngle );
}

void CNode::Pitch( float fAngle )
{
	m_oTransformMatrix = m_oTransformMatrix * CMatrix::GetxRotation( fAngle );
}

void CNode::Roll(float fAngle)
{
	m_oTransformMatrix = m_oTransformMatrix * CMatrix::GetzRotation( fAngle );
}

void CNode::SetWorldPosition( float x, float y, float z )
{
  	m_oTransformMatrix.m_a14 = x;
  	m_oTransformMatrix.m_a24 = y;
  	m_oTransformMatrix.m_a34 = z;
}

CVector CNode::GetWorldPosition() const
{
	return CVector( m_oTransformMatrix.m_a14 , m_oTransformMatrix.m_a24 , m_oTransformMatrix.m_a34, m_oTransformMatrix.m_a44 );
}


void CNode::SetName( const string& sName )
{
	m_sName = sName;
}


void CNode::GetName( string& sName ) const
{
	sName = m_sName;
}

void CNode::SetTransformMatrix( const CMatrix& oMat )
{
	m_oTransformMatrix = oMat;
}

void CNode::GetTransformMatrix( CMatrix& oMat )
{
	oMat = m_oTransformMatrix;
}