#include "Entity.h"
#include "Scene.h"

// stl
#include <algorithm>

// engine
#include "IRenderer.h"
#include "IRessource.h"
#include "ISystems.h"
#include "IShader.h"
#include "IGeometry.h"
#include "TimeManager.h"
#include "ICollisionManager.h"

// Utils

using namespace std;

CEntity::CEntity( IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IGeometryManager& oGeometryManager, ICollisionManager& oCollisionManager ):
m_oRessourceManager( oRessourceManager ),
m_oRenderer( oRenderer ),
m_pCurrentAnimation( NULL ),
m_pOrgSkeletonRoot( NULL ),
m_pSkeletonRoot( NULL ),
m_bHidden( false ),
m_pEntityRoot( NULL ),
m_pEntityManager( pEntityManager ),
m_bUsePositionKeys( false ),
m_eRenderType( IRenderer::eFill ),
m_pBoundingSphere( NULL ),
m_fBoundingSphereRadius( -1 ),
m_pBBox( NULL ),
m_oGeometryManager( oGeometryManager ),
m_bDrawAnimationBoundingBox( false ),
m_oCollisionManager( oCollisionManager ),
m_pfnCollisionCallback( NULL )
{
}

CEntity::CEntity( const string& sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IGeometryManager& oGeometryManager, ICollisionManager& oCollisionManager, bool bDuplicate ):
m_oRenderer( oRenderer ),
m_oRessourceManager( oRessourceManager ),
m_pCurrentAnimation( NULL ),
m_pOrgSkeletonRoot( NULL ),
m_pSkeletonRoot( NULL ),
m_bHidden( false ),
m_pEntityRoot( NULL ),
m_pEntityManager( pEntityManager ),
m_bUsePositionKeys( false ),
m_eRenderType( IRenderer::eFill ),
m_pBoundingSphere( NULL ),
m_fBoundingSphereRadius( -1 ),
m_pBBox( NULL ),
m_oGeometryManager( oGeometryManager ),
m_bDrawAnimationBoundingBox( false ),
m_oCollisionManager( oCollisionManager ),
m_pfnCollisionCallback( NULL )
{
	if( sFileName.size() > 0 )
	{
		SetRessource( sFileName, oRessourceManager, oRenderer );
		m_fBoundingSphereRadius = m_pBBox->ComputeBoundingSphereRadius();
	}
}

CEntity::~CEntity()
{
}

float CEntity::GetBoundingSphereRadius() const
{
	return m_fBoundingSphereRadius;
}

void CEntity::SetEntityRoot( CEntity* pRoot )
{
	m_pEntityRoot = pRoot;
}

CEntity* CEntity::GetEntityRoot()
{
	return m_pEntityRoot;
}

void CEntity::SetRenderingType( IRenderer::TRenderType t )
{
	m_eRenderType = t;
}


void CEntity::SetRessource( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, bool bDuplicate )
{
	int nPos = static_cast< int >( sFileName.find_last_of( "." ) );
	string sExt = sFileName.substr( nPos + 1, 3 );
	string sExtLower = sExt;
	transform( sExt.begin(), sExt.end(), sExtLower.begin(), tolower );
	if ( sExtLower == "ale" )
		bDuplicate = true;
	IAnimatableMesh* pAMesh = dynamic_cast< IAnimatableMesh* >( oRessourceManager.GetRessource( sFileName, oRenderer, bDuplicate ) );
	if( pAMesh )
	{
		if( pAMesh->GetMeshCount() > 0 )
		{
			m_pRessource = pAMesh->GetMesh( 0 );
			m_pRessource->GetName( m_sName );
			m_pOrgSkeletonRoot = pAMesh->GetSkeleton();
			if( m_pOrgSkeletonRoot )
				m_pSkeletonRoot = static_cast< IBone* >( m_pOrgSkeletonRoot->DuplicateHierarchy() );

			for( unsigned int iMesh = 0; iMesh < pAMesh->GetMeshCount(); iMesh++ )
			{
				IMesh* pMesh = pAMesh->GetMesh( iMesh );
				if( pMesh->GetParentBoneID() != -1 )
				{
					IBone* pParentBone = static_cast< IBone* >( m_pSkeletonRoot->GetChildBoneByID( pMesh->GetParentBoneID() ) );
					string sName;
					pMesh->GetName( sName );
					CEntity* pEntity = static_cast< CEntity* >( m_pEntityManager->CreateEntity( sName ) );
					pEntity->SetMesh( pMesh );
					
					pEntity->SetName( sName );
					LinkEntityToBone( pEntity, pParentBone );
				}
			}
			m_pBBox = GetBBox();
		}
	}
	else
	{
		CEException e( "Nous vous autorisons à foueter le développeur" );
		throw e;
	}
}

void CEntity::UpdateGroundCollision()
{
	m_oBody.Update();
	if( m_oBody.m_fWeight > 0.f )
	{
		if( m_pScene )
		{
			CVector oTransformedBoxPosition = m_oLocalMatrix.GetRotation() * m_oScaleMatrix * m_pBBox->GetMinPoint();
			float x = 0.f;
			float z = 0.f;

			if( !m_bUsePositionKeys )
			{
				x = m_oLocalMatrix.m_03;
				z = m_oLocalMatrix.m_23;
			}
			else
			{
				CMatrix oSkeletonRootMatrix;
				m_pSkeletonRoot->GetLocalMatrix( oSkeletonRootMatrix );
				CMatrix oSkeletonOffset = m_oFirstAnimationFrameSkeletonMatrixInv * oSkeletonRootMatrix;
				CMatrix oComposedMatrix = oSkeletonOffset * m_oLocalMatrix;
				x = oComposedMatrix.m_03;
				z = oComposedMatrix.m_23;
			}
			float fHeight = m_pScene->GetHeight( x, z ) + CBody::GetZCollisionError();
			float fEntityZ = m_oLocalMatrix.m_13 + oTransformedBoxPosition.m_y;
			if( fEntityZ > fHeight + CBody::GetEpsilonError() )
			{
				int nDelta = CTimeManager::Instance()->GetTimeElapsedSinceLastUpdate();
				if( nDelta != 0 )
				{
					CVector vTranslation = m_oBody.m_oSpeed * ((float)nDelta / 1000.f);
					WorldTranslate( vTranslation );
				}
			}
			else
			{
				m_oBody.m_oSpeed.m_x = 0;
				m_oBody.m_oSpeed.m_y = 0;
				m_oBody.m_oSpeed.m_z = 0;
				if( fEntityZ < fHeight + CBody::GetEpsilonError() )
					SetWorldPosition( m_oLocalMatrix.m_03, fHeight - oTransformedBoxPosition.m_y, m_oLocalMatrix.m_23  );
			}
		}
	}
}

IEntity* CEntity::GetEntityCollision()
{
	IEntity* pEntity = m_pEntityManager->GetFirstCollideEntity();
	while( pEntity )
	{
		if( pEntity == this )
		{
			pEntity = m_pEntityManager->GetNextCollideEntity();
			continue;
		}
		CVector oThisPosition, oEntityWorldPosition;
		GetWorldPosition( oThisPosition );
		pEntity->GetWorldPosition( oEntityWorldPosition );
		float fDistance = ( oThisPosition - oEntityWorldPosition ).Norm() - ( m_fBoundingSphereRadius / 2.f ) - ( pEntity->GetBoundingSphereRadius() / 2.f );
		if( fDistance < 0 )
		{
			IMesh* pCurrentMesh = static_cast< IMesh* >( m_pRessource );
			IMesh* pOtherMesh = static_cast< IMesh* >( pEntity->GetRessource() );
			IBox* pCurrentBox = NULL, *pOtherBox = NULL;
			string sAnimationName;
			if( m_pCurrentAnimation )
			{
				m_pCurrentAnimation->GetName( sAnimationName );
				pCurrentBox = m_oGeometryManager.CreateBox( *pCurrentMesh->GetAnimationBBox( sAnimationName ) );
			}
			else
				pCurrentBox = m_oGeometryManager.CreateBox( *pCurrentMesh->GetBBox() );

			if( pEntity->GetCurrentAnimation() )
			{
				pEntity->GetCurrentAnimation()->GetName( sAnimationName );
				pOtherBox = m_oGeometryManager.CreateBox( *pOtherMesh->GetAnimationBBox( sAnimationName ) );
			}
			else
				pOtherBox = m_oGeometryManager.CreateBox( *pOtherMesh->GetBBox() );

			pCurrentBox->SetWorldMatrix( m_oWorldMatrix );
			CMatrix oOtherWorldMatrix;
			pEntity->GetWorldMatrix( oOtherWorldMatrix );
			pOtherBox->SetWorldMatrix( oOtherWorldMatrix );
			if( m_oCollisionManager.IsIntersection( *pCurrentBox, *pOtherBox ) )
				return pEntity;
			
		}
		pEntity = m_pEntityManager->GetNextCollideEntity();
	}
	return NULL;
}

void CEntity::Update()
{
	UpdateGroundCollision();

	if( m_pCurrentAnimation )
		m_pCurrentAnimation->Update();

	CNode::Update();

	if( m_pSkeletonRoot )
		m_pSkeletonRoot->Update();

	if( m_pEntityRoot )
	{
		CMatrix oRootMatrix;
		m_pEntityRoot->GetWorldMatrix( oRootMatrix );
		m_oWorldMatrix = oRootMatrix * m_oWorldMatrix;
	}

	if( m_pOrgSkeletonRoot )
	{
		vector< CMatrix > vBoneMatrix;
		GetBonesMatrix( m_pOrgSkeletonRoot, m_pSkeletonRoot, vBoneMatrix );
		SetNewBonesMatrixArray( vBoneMatrix );
	}
	m_oWorldMatrix *= m_oScaleMatrix;
	m_oRenderer.SetObjectMatrix( m_oWorldMatrix );

	if( !m_bHidden )
	{
		IMesh* pMesh = dynamic_cast< IMesh* >( m_pRessource );
		if( pMesh )
		{
			pMesh->SetRenderingType( m_eRenderType );
			pMesh->DrawAnimationBoundingBox( m_bDrawAnimationBoundingBox );
		}
		m_pRessource->Update();
		if( pMesh )
			pMesh->SetRenderingType( IRenderer::eFill );
	}
}

void CEntity::LocalTranslate(float dx , float dy , float dz)
{
	if( m_pEntityManager )
	{
		int nID = m_pEntityManager->GetCollideEntityID( this );
		if( ( nID != -1 ) && ( m_fBoundingSphereRadius > 0 ) )
		{
			bool bCollision = false;
			IEntity* pEntity = GetEntityCollision();
			if( pEntity )
			{
				CVector oThisPos, oEntityPos;
				GetWorldPosition( oThisPos );
				pEntity->GetWorldPosition( oEntityPos );
				float fCurrentDistance = ( oThisPos - oEntityPos ).Norm();
				CMatrix oTemp = m_oLocalMatrix;
				CNode::LocalTranslate( dx, dy, dz );
				CNode::UpdateWithoutChildren();
				GetWorldPosition( oThisPos );
				pEntity->GetWorldPosition( oEntityPos );
				float fNextDistance = ( oThisPos - oEntityPos ).Norm();
				if( fNextDistance < fCurrentDistance )
				{
					m_oLocalMatrix = oTemp;
					bCollision = true;
				}
			}
			else
			{
				CMatrix oTemp = m_oLocalMatrix;
				CNode::LocalTranslate( dx, dy, dz );
				CNode::UpdateWithoutChildren();
				if( GetEntityCollision() )
				{
					m_oLocalMatrix = oTemp;
					bCollision = true;
				}
			}
			if( bCollision && m_pfnCollisionCallback )
				m_pfnCollisionCallback( this );
		}
		else
			CNode::LocalTranslate( dx, dy, dz );
	}
	else
		CNode::LocalTranslate( dx, dy, dz );
}

void CEntity::LocalTranslate( const CVector& vTranslate )
{
	LocalTranslate( vTranslate.m_x, vTranslate.m_y, vTranslate.m_z );
}

void CEntity::LinkEntityToBone( IEntity* pChild, IBone* pParentBone, IEntity::TLinkType t )
{
	CMatrix oParentWorld;
	pParentBone->GetWorldMatrix( oParentWorld );
	if( t == ePreserveChildRelativeTM )
	{
		CMatrix oParentWorldInv;		
		oParentWorld.GetInverse( oParentWorldInv );
		pChild->SetLocalMatrix( oParentWorldInv );
	}
	else if (  t = eSetChildToParentTM )
	{
		CMatrix oIdentity;
		pChild->SetLocalMatrix( oIdentity );
	}
	pChild->Link( pParentBone );
	CEntity* pChildEntity = static_cast< CEntity* >( pChild );
	pChildEntity->SetEntityRoot( this );
	IMesh* pMesh = dynamic_cast< IMesh* >( pChild->GetRessource() );
	pChild->LocalTranslate( pMesh->GetOrgMaxPosition() );
}

void CEntity::Link( CNode* pParent )
{
	CNode::Link( pParent );
	m_pScene = dynamic_cast< CScene* >( pParent );
	if( m_pEntityManager )
	{
		if( m_pScene && m_fBoundingSphereRadius > 0.f )
			m_pEntityManager->AddCollideEntity( this );
		else
			m_pEntityManager->RemoveCollideEntity( this );
	}
}

void CEntity::Hide( bool bHide )
{
	m_bHidden = bHide;
}

void CEntity::GetBonesMatrix( CNode* pInitRoot, CNode* pCurrentRoot, vector< CMatrix >& vMatrix )
{
	// m0 = base du node dans sa position initiale, m0i = inverse de m0
	// m1 = base du node dans sa position actuelle
	// m2 = matrice de passage de m0 à m1 (m2 = inv(m0)*m1)
	CMatrix m0, m1, mPassage, m0i;
	pInitRoot->GetWorldMatrix( m0 );
	pCurrentRoot->GetWorldMatrix( m1 );
	CMatrix::GetPassage( m0, m1, mPassage );
	vMatrix.push_back( mPassage );
	for ( unsigned int i = 0; i < pInitRoot->GetChildCount(); i++ )
	{
		if ( pInitRoot->GetChild( i ) )
			GetBonesMatrix( pInitRoot->GetChild( i ), pCurrentRoot->GetChild( i ), vMatrix );
	}
}

void CEntity::SetNewBonesMatrixArray( std::vector< CMatrix >& vMatBones )
{
	m_pRessource->GetCurrentShader()->SendUniformMatrix4Array( "matBones", vMatBones, true );
}

float CEntity::GetWeight()
{
	return m_oBody.m_fWeight;
}

void CEntity::SetWeight( float fWeight )
{
	m_oBody.m_fWeight = fWeight;
}


void CEntity::OnAnimationCallback( IAnimation::TEvent e, void* pData )
{
	switch( e )
	{
	case IAnimation::ePlay:
		CEntity* pEntity = reinterpret_cast< CEntity* >( pData );
		IBone* pRoot = pEntity->m_pSkeletonRoot;
		
		CKey oKey;
		pRoot->GetKeyByTime( pEntity->GetCurrentAnimation()->GetStartAnimationTime(), oKey );
		oKey.m_oLocalTM.GetInverse( pEntity->m_oFirstAnimationFrameSkeletonMatrixInv );
		break;
	}
}


void CEntity::SetMesh( IMesh* pMesh )
{
	m_pRessource = pMesh;
}

void CEntity::DrawBoundingBox( bool bDraw )
{
	IMesh* pMesh = dynamic_cast< IMesh* >( m_pRessource );
	if( pMesh )
		pMesh->DrawBoundingBox( bDraw );
}

void CEntity::SetShader( IShader* pShader )
{
	m_pRessource->SetShader( pShader );
}

void CEntity::CenterToworld()
{
#if 0
	IBox* pBbox = m_oGeometryManager.CreateBox();
	static_cast< IMesh* >( m_pRessource )->GetBbox( *pBbox );
#endif // 0
	throw 1;
}

IBox* CEntity::GetBBox()
{
	return static_cast< IMesh* >( m_pRessource )->GetBBox();
}

IRessource*	CEntity::GetRessource()
{
	return m_pRessource;
}

IAnimation* CEntity::GetCurrentAnimation()
{
	return m_pCurrentAnimation;
}

void CEntity::AddAnimation( string sAnimationFile )
{
	if( m_pSkeletonRoot )
	{
		IAnimation* pAnimation =  static_cast< IAnimation* >( m_oRessourceManager.GetRessource( sAnimationFile, m_oRenderer, true ) );
		m_mAnimation[ sAnimationFile ] =  pAnimation;
		IMesh* pMesh = static_cast< IMesh* >( m_pRessource );
		pAnimation->SetSkeleton( m_pSkeletonRoot );
	}
	else
	{
		ostringstream oss;
		oss << "CEntity::AddAnimation() : Erreur : l'entité numéro " << m_nID << " n'est pas animable.";
		CEException e( oss.str() );
		throw e;
	}
}

void CEntity::SetCurrentAnimation( std::string sAnimation )
{
	m_sCurrentAnimation = sAnimation;
	m_pCurrentAnimation = m_mAnimation[ sAnimation ];
	if( m_bUsePositionKeys )
		m_pCurrentAnimation->AddCallback( OnAnimationCallback, this );
}

bool CEntity::HasAnimation( string sAnimationName )
{
	map< string, IAnimation* >::iterator itAnim = m_mAnimation.find( sAnimationName );
	return itAnim != m_mAnimation.end();
}

IBone* CEntity::GetSkeletonRoot()
{
	return m_pSkeletonRoot;
}

void CEntity::DetachCurrentAnimation()
{
	m_pCurrentAnimation = NULL;
}

void CEntity::GetTypeName( string& sName )
{
	sName = m_sTypeName;
}

void CEntity::SetScaleFactor( float x, float y, float z )
{
	m_oScaleMatrix.m_00 = x;
	m_oScaleMatrix.m_11 = y;
	m_oScaleMatrix.m_22 = z;
}

void CEntity::DrawBoundingSphere( bool bDraw )
{
	IBone* pSkeleton = GetSkeletonRoot();
	if( bDraw )
	{
		if( !m_pBoundingSphere )
			m_pBoundingSphere = m_pEntityManager->CreateEntity( "sphere.bme", "", m_oRenderer );
		if( pSkeleton )
			LinkEntityToBone( m_pBoundingSphere, pSkeleton );
		else
			m_pBoundingSphere->Link( this );
		m_pBoundingSphere->SetScaleFactor( m_fBoundingSphereRadius / 2.f, m_fBoundingSphereRadius / 2.f, m_fBoundingSphereRadius / 2.f );
		m_pBoundingSphere->SetRenderingType( IRenderer::eLine );
	}
	else
		m_pBoundingSphere->Unlink();
}

void CEntity::DrawBoneBoundingSphere( int nID, bool bDraw )
{
	IBone* pBone = dynamic_cast< IBone* >( m_pSkeletonRoot->GetChildBoneByID( nID ) );
	if( pBone )
	{
		map< int, IEntity* >::iterator itBone = m_mBonesBoundingSphere.find( nID );
		if( itBone == m_mBonesBoundingSphere.end() )
			m_mBonesBoundingSphere[ nID ] = m_pEntityManager->CreateEntity( "sphere.bme", "", m_oRenderer );
		if( bDraw )
		{		
			if( pBone )
				LinkEntityToBone( m_mBonesBoundingSphere[ nID ], pBone, eSetChildToParentTM );
			else
				throw 1;
			float fDiameter = pBone->GetBoundingBox()->ComputeBoundingSphereRadius();
			m_mBonesBoundingSphere[ nID ]->SetScaleFactor( fDiameter / 2.f, fDiameter / 2.f, fDiameter / 2.f );
			m_mBonesBoundingSphere[ nID ]->SetRenderingType( IRenderer::eLine );
		}
		else
			m_mBonesBoundingSphere[ nID ]->Unlink();
	}
	else
	{
		ostringstream oss;
		oss << "Erreur : il n'existe pas de bone ayant l'identifiant " << nID;
		CEException e( "" );
	}
}

void CEntity::DrawAnimationBoundingBox( bool bDraw )
{
	m_bDrawAnimationBoundingBox = bDraw;
	IMesh* pMesh = dynamic_cast< IMesh* >( m_pRessource );
	if( pMesh && bDraw )
	{
		string sAnimationName;
		GetCurrentAnimation()->GetName( sAnimationName );
		pMesh->SetCurrentAnimationBoundingBox( sAnimationName );
	}
}

void CEntity::Goto( const CVector& oPosition, float fSpeed )
{
	CEException e( "La méthode \"Goto\" n'est pas encore implémentée pour les entité génériques. Veuillez l'utiliser uniquement pour les entité de type \"IAEntity\"" );
	throw e;
}

void CEntity::SetEntityName( string sName )
{
	m_sEntityName = sName;
}

void CEntity::GetEntityName( string& sName )
{
	sName = m_sEntityName;
}