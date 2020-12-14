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
#include "Utils2/TimeManager.h"
#include "ICollisionManager.h"
#include "EntityManager.h"
#include "IGUIManager.h"

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
m_bUsePositionKeys( false ),
m_eRenderType( IRenderer::eFill ),
m_pBoundingSphere( NULL ),
m_fBoundingSphereRadius( -1 ),
m_oGeometryManager( oGeometryManager ),
m_bDrawAnimationBoundingBox( false ),
m_oCollisionManager( oCollisionManager ),
m_pfnCollisionCallback( NULL ),
m_bUseAdditionalColor(false),
m_pBoundingGeometry(NULL),
m_pRessource(NULL),
m_fMaxStepHeight(50.f)
{
	m_pEntityManager = static_cast<CEntityManager*>(pEntityManager);
}

CEntity::CEntity( const string& sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, IGeometryManager& oGeometryManager, ICollisionManager& oCollisionManager, bool bDuplicate ):
m_oRenderer( oRenderer ),
m_oRessourceManager( oRessourceManager ),
m_pCurrentAnimation( NULL ),
m_pOrgSkeletonRoot( NULL ),
m_pSkeletonRoot( NULL ),
m_bHidden( false ),
m_pEntityRoot( NULL ),
m_pEntityManager( (CEntityManager*)pEntityManager ),
m_bUsePositionKeys( false ),
m_eRenderType( IRenderer::eFill ),
m_pBoundingSphere( NULL ),
m_fBoundingSphereRadius( -1 ),
m_oGeometryManager( oGeometryManager ),
m_bDrawAnimationBoundingBox( false ),
m_oCollisionManager( oCollisionManager ),
m_pfnCollisionCallback( NULL ),
m_bUseAdditionalColor(false),
m_pCollisionMesh(NULL),
m_pRessource(NULL),
m_pBoundingGeometry(NULL),
m_fMaxStepHeight(50.f)
{
	if( sFileName.size() > 0 )
	{
		SetRessource( sFileName, oRessourceManager, oRenderer );
		if (m_pBoundingGeometry)
			m_fBoundingSphereRadius = m_pBoundingGeometry->ComputeBoundingSphereRadius();
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
			m_pBoundingGeometry = GetBBox();
			CreateAndLinkCollisionChildren(sFileName);
		}
	}
	else
	{
		CEException e( "Nous vous autorisons à fouetter le développeur" );
		throw e;
	}
}

void CEntity::CreateAndLinkCollisionChildren(string sFileName)
{
	int dotPos = sFileName.find('.');
	string sCollisionFileName = sFileName.substr(0, dotPos) + ".col";
	try {
		m_pCollisionMesh = dynamic_cast<ICollisionMesh*>(m_oRessourceManager.GetRessource(sCollisionFileName, m_oRenderer, false));
		for (int i = 0; i < m_pCollisionMesh->GetGeometryCount(); i++) {
			IGeometry* pGeometry = m_pCollisionMesh->GetGeometry(i);
			CEntity* pChild = new CEntity(m_oRessourceManager, m_oRenderer, m_pEntityManager, m_oGeometryManager, m_oCollisionManager);
			pChild->ForceAssignBoundingGeometry(pGeometry);
			pChild->Link(this);
		}
	}
	catch (CFileNotFoundException& e) {

	}
}

void CEntity::UpdateCollision()
{
	m_oBody.Update();
	if( m_oBody.m_fWeight > 0.f )
	{
		if( m_pScene )
		{
			CVector oBase;
			m_pBoundingGeometry->GetBase(oBase);
			CVector oTransformedBoxPosition = m_oLocalMatrix.GetRotation() * m_oScaleMatrix * oBase;

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
			int nDelta = CTimeManager::Instance()->GetTimeElapsedSinceLastUpdate();
			float fGroundHeight = m_pScene->GetHeight(x, z) + CBody::GetZCollisionError();
			float fEntityZ = m_oLocalMatrix.m_13 + oTransformedBoxPosition.m_y + m_oBody.m_oSpeed.m_y * (float)nDelta / 1000.f;
			if( fEntityZ > fGroundHeight + CBody::GetEpsilonError() )
			{
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
				if( fEntityZ < fGroundHeight + CBody::GetEpsilonError() )
					SetWorldPosition( m_oLocalMatrix.m_03, fGroundHeight - oTransformedBoxPosition.m_y, m_oLocalMatrix.m_23  );
			}
		}
	}
}

CEntity* CEntity::GetEntityCollision()
{
	for (int i = 0; i < m_pParent->GetChildCount(); i++) {
		CEntity* pEntity = dynamic_cast<CEntity*>(m_pParent->GetChild(i));
		if (!pEntity || pEntity == this)
			continue;

		if (GetBoundingSphereDistance(pEntity) < 0)
		{
			IBox* pCurrentGeometry = static_cast<IBox*>(GetBoundingGeometry()->Duplicate());
			IBox* pOtherGeometry = static_cast<IBox*>(pEntity->GetBoundingGeometry()->Duplicate());

			pCurrentGeometry->SetWorldMatrix(m_oWorldMatrix);
			CMatrix oOtherWorldMatrix;
			pEntity->GetWorldMatrix(oOtherWorldMatrix);
			pOtherGeometry->SetWorldMatrix(oOtherWorldMatrix);

			if (m_oCollisionManager.IsIntersection(*pCurrentGeometry, *pOtherGeometry))
				return pEntity;

		}
	}
	return NULL;
}

float CEntity::GetBoundingSphereDistance(CEntity* pEntity)
{
	CVector oThisPosition, oEntityWorldPosition;
	GetWorldPosition(oThisPosition);
	pEntity->GetWorldPosition(oEntityWorldPosition);
	float fDistance = (oThisPosition - oEntityWorldPosition).Norm() - (m_fBoundingSphereRadius / 2.f) - (pEntity->GetBoundingSphereRadius() / 2.f);
	return fDistance;
}

void CEntity::Update()
{
	UpdateCollision();

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
			if (m_bUseAdditionalColor)
				pMesh->Colorize(m_oAdditionalColor.m_x, m_oAdditionalColor.m_y, m_oAdditionalColor.m_z, m_oAdditionalColor.m_w);
		}
		if(m_pRessource)
			m_pRessource->Update();
		if( pMesh )
			pMesh->SetRenderingType( IRenderer::eFill );
	}	
}

float CEntity::GetHeight()
{
	IGeometry* pGeometry = m_pBoundingGeometry->Duplicate();
	CMatrix tm = m_oLocalMatrix.GetRotation() * m_oScaleMatrix;
	pGeometry->Transform(tm);
	return pGeometry->GetHeight();
}

void CEntity::LocalTranslate(float dx, float dy, float dz)
{
	float fStepHeight = dy != 0 ? 50.f : 0.f;
	if (m_pEntityManager)
	{
		int nID = m_pEntityManager->GetCollideEntityID(this);
		if ((nID != -1) && (m_fBoundingSphereRadius > 0))
		{
			bool bCollision = false;
			CVector oThisPos, oEntityPos;
			GetWorldPosition(oThisPos);

			CEntity* pEntity = GetEntityCollision();
			if (pEntity)
			{
				pEntity->GetWorldPosition(oEntityPos);
				float fCurrentDistance = (oThisPos - oEntityPos).Norm();
				CMatrix oTemp = m_oLocalMatrix;
				CNode::LocalTranslate(dx, dy, dz);
				CNode::UpdateWithoutChildren();
				GetWorldPosition(oThisPos);
				pEntity->GetWorldPosition(oEntityPos);
				float fNextDistance = (oThisPos - oEntityPos).Norm();
				if (fNextDistance < fCurrentDistance)
				{
					if (dx == 0 && dy == 0 && dz != 0) {
						float h = GetHeight();
						float collideEntityHeight = pEntity->GetHeight() / 2.f + oEntityPos.m_y;
						if (collideEntityHeight < oThisPos.m_y + m_fMaxStepHeight)
						{
							float newy = h / 2.f + collideEntityHeight; // -CBody::GetZCollisionError();
							SetWorldPosition(oThisPos.m_x, newy, oThisPos.m_z);
							m_oBody.m_oSpeed.m_y = 0;
							UpdateWithoutChildren();
						}
					}
					else if (pEntity->GetHeight() > fStepHeight) {
						m_oLocalMatrix = oTemp;
						bCollision = true;
					}
				}
				else
					TestGroundCollision(oTemp);
			}
			else
			{
				CMatrix oTemp = m_oLocalMatrix;
				CNode::LocalTranslate(dx, dy, dz);
				CNode::UpdateWithoutChildren();
				pEntity = GetEntityCollision();
				if (pEntity)
				{
					pEntity->GetWorldPosition(oEntityPos);
					if (dx == 0 && dy == 0 && dz != 0) {
						float h = GetHeight();
						float collideEntityHeight = pEntity->GetHeight() / 2.f + oEntityPos.m_y;
						if (collideEntityHeight < oThisPos.m_y + m_fMaxStepHeight)
						{
							float newy = h / 2.f + collideEntityHeight; // -CBody::GetZCollisionError();
							SetWorldPosition(oThisPos.m_x, newy, oThisPos.m_z);
							m_oBody.m_oSpeed.m_y = 0;
							UpdateWithoutChildren();
						}
					}
					else if (pEntity->GetHeight() > fStepHeight) {
						m_oLocalMatrix = oTemp;
						bCollision = true;
					}
				}
				else {
					TestGroundCollision(oTemp);
				}
			}
			if (bCollision && m_pfnCollisionCallback) {
				m_pfnCollisionCallback(this, pEntity);
			}
		}
		else
			CNode::LocalTranslate(dx, dy, dz);
	}
	else
		CNode::LocalTranslate(dx, dy, dz);
}

bool CEntity::TestGroundCollision(const CMatrix& olastLocalTM)
{
	// height map collision
	bool bCollision = false;
	float h = GetHeight();
	float x, y, z;
	m_oWorldMatrix.GetPosition(x, y, z);
	float fGroundHeight = m_pScene->GetHeight(x, z) + CBody::GetZCollisionError();
	float fEntityY = y - h / 2.f;
	if (fEntityY <= fGroundHeight + CBody::GetEpsilonError()) {
		m_oBody.m_oSpeed.m_x = 0;
		m_oBody.m_oSpeed.m_y = 0;
		m_oBody.m_oSpeed.m_z = 0;
		SetWorldPosition(x, fGroundHeight + h / 2.f, z);
	}
	return bCollision;
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

void CEntity::Colorize(float r, float g, float b, float a)
{
	IMesh* pMesh = dynamic_cast<IMesh*>(m_pRessource);
	if (pMesh) {
		m_bUseAdditionalColor = true;
		m_oAdditionalColor.m_x = r;
		m_oAdditionalColor.m_y = g;
		m_oAdditionalColor.m_z = b;
		m_oAdditionalColor.m_w = a;
	}
}

ICollisionMesh* CEntity::GetCollisionMesh()
{
	return m_pCollisionMesh;
}

void CEntity::ForceAssignBoundingGeometry(IGeometry* pBoundingGeometry)
{
	m_pBoundingGeometry = pBoundingGeometry;
}

IGeometry* CEntity::GetBoundingGeometry()
{
	IGeometry* pGeometry = NULL;
	if (GetCurrentAnimation())
	{
		string sAnimationName;
		GetCurrentAnimation()->GetName(sAnimationName);
		IMesh* pMesh = static_cast< IMesh* >(GetRessource());
		pGeometry = pMesh->GetAnimationBBox(sAnimationName);
	}
	else
		pGeometry = m_pBoundingGeometry;

	return pGeometry;
}
