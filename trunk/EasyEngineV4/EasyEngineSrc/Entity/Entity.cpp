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
#include "ICollisionManager.h"
#include "EntityManager.h"
#include "IGUIManager.h"

// Utils
#include "Utils2/TimeManager.h"
#include "Utils2/RenderUtils.h"

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
m_fMaxStepHeight(50.f),
m_pCollisionMesh(NULL),
m_bDrawBoundingBox(false)
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
m_fMaxStepHeight(50.f),
m_bDrawBoundingBox(false)
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
			if (m_pOrgSkeletonRoot) {
				m_pSkeletonRoot = static_cast<IBone*>(m_pOrgSkeletonRoot->DuplicateHierarchy());
				m_pSkeletonRoot->Link(this);
			}

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
			IMesh* pMesh = static_cast<IMesh*>(m_pRessource);
			IBox* pAnimationBBox = pMesh->GetAnimationBBox("stand");
			if (pAnimationBBox)
				m_pBoundingGeometry = pAnimationBBox;
			else
				m_pBoundingGeometry = pMesh->GetBBox();

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
	string sPrefix = sFileName.substr(0, dotPos);
	string sCollisionFileName = sPrefix + ".col";
	try {
		m_pCollisionMesh = dynamic_cast<ICollisionMesh*>(m_oRessourceManager.GetRessource(sCollisionFileName, m_oRenderer, false));
		for (int i = 0; i < m_pCollisionMesh->GetGeometryCount(); i++) {
			IGeometry* pGeometry = m_pCollisionMesh->GetGeometry(i);
			ostringstream oss;
			oss << sPrefix << "_CollisionPrimitive" << i;
			CEntity* pChild = static_cast<CEntity*>(m_pEntityManager->CreateEntity(oss.str()));
			pChild->SetLocalMatrix(pGeometry->GetTM());
			pChild->ForceAssignBoundingGeometry(pGeometry);
			pChild->m_fBoundingSphereRadius = pGeometry->ComputeBoundingSphereRadius();
			pChild->Link(this);
			pChild->SetEntityName(oss.str());
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
			CVector oTransformedBoxPosition = m_oLocalMatrix.GetRotation() * m_oScaleMatrix * m_pBoundingGeometry->GetBase();

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
					SetLocalPosition( m_oLocalMatrix.m_03, fGroundHeight - oTransformedBoxPosition.m_y, m_oLocalMatrix.m_23  );
			}
		}
	}
}

bool CEntity::TestEntityCollision(CEntity* pEntity)
{
	if (GetBoundingSphereDistance(pEntity) < 0)
	{
		IGeometry* pCurrentGeometry = GetBoundingGeometry()->Duplicate();
		IGeometry* pOtherGeometry = pEntity->GetBoundingGeometry() ? pEntity->GetBoundingGeometry()->Duplicate() : NULL;

		if (pOtherGeometry) {
			pCurrentGeometry->SetTM(m_oWorldMatrix);
			CMatrix oOtherWorldMatrix;
			pEntity->GetWorldMatrix(oOtherWorldMatrix);
			pOtherGeometry->SetTM(oOtherWorldMatrix);
			return pCurrentGeometry->IsIntersect(*pOtherGeometry);
		}
	}
	return false;
}

void CEntity::LinkAndUpdateMatrices(CEntity* pEntity)
{
	CMatrix tm, tmInv, tmThis;
	pEntity->GetWorldMatrix(tm);
	tm.GetInverse(tmInv);
	GetWorldMatrix(tmThis);
	SetLocalMatrix(tmInv * tmThis);
	Link(pEntity);
	//pEntity->Update();
}

CEntity* CEntity::GetEntityCollision()
{
	for (int i = 0; i < m_pParent->GetChildCount(); i++) {
		CEntity* pEntity = dynamic_cast<CEntity*>(m_pParent->GetChild(i));
		if (!pEntity || pEntity == this)
			continue;

		if (TestEntityCollision(pEntity))
			return pEntity;
	}
	return NULL;
}

void CEntity::GetEntitiesCollision(vector<CEntity*>& entities)
{
	for (int i = 0; i < m_pParent->GetChildCount(); i++) {
		CEntity* pEntity = dynamic_cast<CEntity*>(m_pParent->GetChild(i));
		if (!pEntity || pEntity == this)
			continue;

		if (TestEntityCollision(pEntity))
			entities.push_back(pEntity);
	}
}

float CEntity::GetBoundingSphereDistance(CEntity* pEntity)
{
	CVector oThisPosition, oEntityWorldPosition;
	GetWorldPosition(oThisPosition);
	pEntity->GetWorldPosition(oEntityWorldPosition);
	float fDistance = (oThisPosition - oEntityWorldPosition).Norm() - (m_fBoundingSphereRadius) - (pEntity->GetBoundingSphereRadius());
	return fDistance;
}

void CEntity::Update()
{
	UpdateCollision();

	if( m_pCurrentAnimation )
		m_pCurrentAnimation->Update();

	CNode::Update();

	string sEntityName;
	GetEntityName(sEntityName);
	if (sEntityName.find("perso") != -1) {
		CVector pos;
		GetWorldPosition(pos);
		ostringstream oss;
		oss << "Perso position = (" << pos.m_x << ", " << pos.m_y << ", " << pos.m_z << ")";
		m_pEntityManager->GetGUIManager()->Print(oss.str(), 1000, 10);
	}

	if( m_pOrgSkeletonRoot )
	{
		vector< CMatrix > vBoneMatrix;
		GetBonesMatrix( m_pOrgSkeletonRoot, m_pSkeletonRoot, vBoneMatrix );
		SetNewBonesMatrixArray( vBoneMatrix );
	}
	m_oWorldMatrix *= m_oScaleMatrix;
	m_oRenderer.SetModelMatrix( m_oWorldMatrix );
	
	if( !m_bHidden )
	{
		IMesh* pMesh = dynamic_cast< IMesh* >(m_pRessource);
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
	if (m_bDrawBoundingBox && m_pBoundingGeometry)
		UpdateBoundingBox();
}

void CEntity::UpdateBoundingBox()
{
	IBox* pBBox = dynamic_cast<IBox*>(m_pBoundingGeometry);
	if(pBBox)
		CRenderUtils::DrawBox(pBBox->GetMinPoint(), pBBox->GetDimension(), m_oRenderer);
}

float CEntity::GetHeight()
{
	IGeometry* pGeometry = m_pBoundingGeometry->Duplicate();
	return pGeometry->GetHeight();
}

float CEntity::GetGroundHeight(int x, int z)
{
	float fGroundHeight = -99.f;

	CEntity* pEntity = this;
	CScene* pScene = NULL;
	float parentHeight = 0;
	do {
		pScene = dynamic_cast<CScene*>(pEntity);
		if (!pScene) {
			CVector parentPos;
			pEntity->GetWorldPosition(parentPos);
			parentHeight -= parentPos.m_y;
			pEntity = dynamic_cast<CEntity*>(pEntity->GetParent());
		}
	} while (pEntity && !pScene);

	fGroundHeight = pScene ? pScene->GetHeight(x, z) : 0;
	fGroundHeight += CBody::GetZCollisionError() + parentHeight;
	return fGroundHeight;
}

void CEntity::LocalTranslate(float dx, float dy, float dz)
{
	if ( (m_fBoundingSphereRadius > 0) && (GetWeight() > 0.f) ) {
		float fStepHeight = dy != 0 ? 50.f : 0.f;
		bool bCollision = false;
		CVector oThisPos, oEntityPos;
		GetWorldPosition(oThisPos);

		CEntity* pEntity = NULL;
		vector<CEntity*> entities;
		GetEntitiesCollision(entities);

		CMatrix oTemp = m_oLocalMatrix;	
		if (!entities.empty())
		{
			for (int i = 0; i < entities.size(); i++) {
				pEntity = entities[i];
				pEntity->GetWorldPosition(oEntityPos);
				float fCurrentDistance = (oThisPos - oEntityPos).Norm();
				CNode::LocalTranslate(dx, dy, dz);
				CNode::UpdateWithoutChildren();
				GetWorldPosition(oThisPos);
				pEntity->GetWorldPosition(oEntityPos);
				float fNextDistance = (oThisPos - oEntityPos).Norm();
				if (fNextDistance < fCurrentDistance) {
					bCollision = ManageBoxCollision(entities, dx, dy, dz, oTemp);
				}
				if (bCollision)
					break;
			}
			if(!bCollision)
				ManageGroundCollision(oTemp);
		}
		else
		{
			CNode::LocalTranslate(dx, dy, dz);
			CNode::UpdateWithoutChildren();
			entities.clear();
			GetEntitiesCollision(entities);
			if (!entities.empty())
			{
				pEntity = entities[0];
				bCollision = ManageBoxCollision(entities, dx, dy, dz, oTemp);
			}
			else
				ManageGroundCollision(oTemp);
		}
		if (bCollision && m_pfnCollisionCallback) {
			m_pfnCollisionCallback(this, pEntity);
		}
	}
	else
		CNode::LocalTranslate(dx, dy, dz);
}

bool CEntity::ManageBoxCollision(vector<CEntity*>& vCollideEntities, float dx, float dy, float dz, const CMatrix& oBackupMatrix)
{
	CEntity* pCollideEntity = vCollideEntities[0];
	float stepHeight = dy != 0 ? 50.f : 0.f;
	bool bCollision = false;
	if (dx == 0 && dy != 0 && dz == 0) {
		float h = GetHeight();
		float collideEntityHeight = pCollideEntity->GetY() + pCollideEntity->GetHeight() / 2.f;
		if (collideEntityHeight < GetY() + m_fMaxStepHeight)
		{
			float newy = h / 2.f + collideEntityHeight;
			for (int i = 1; i < vCollideEntities.size(); i++) {
				pCollideEntity = vCollideEntities[i];
				int collideEntityHeight2 = pCollideEntity->GetHeight() / 2.f + pCollideEntity->GetY();
				if ( (collideEntityHeight2 > collideEntityHeight) && (collideEntityHeight2 < GetY() + m_fMaxStepHeight)) {
					newy = h / 2.f + collideEntityHeight2;
				}
			}

			SetWorldPosition(GetX(), newy, GetZ());
			m_oBody.m_oSpeed.m_y = 0;
			UpdateWithoutChildren();
		}
	}
	else {
		for (int i = 0; i < vCollideEntities.size(); i++) {
			pCollideEntity = vCollideEntities[i];
			int collideEntityHeight = pCollideEntity->GetY() + pCollideEntity->GetHeight() / 2.f;
			if (GetY() + stepHeight < collideEntityHeight) {
				m_oLocalMatrix = oBackupMatrix;
				bCollision = true;
				break;
			}
		}
	}
	return bCollision;
}

bool CEntity::ManageGroundCollision(const CMatrix& olastLocalTM)
{
	bool bCollision = false;
	float h = GetHeight();	
	CVector localPos, worldPos;
	m_oLocalMatrix.GetPosition(localPos);
	m_oWorldMatrix.GetPosition(worldPos);
	float fGroundHeight = static_cast<CEntity*>(m_pParent)->GetGroundHeight(worldPos.m_x, worldPos.m_z);
	float fEntityY = localPos.m_y - h / 2.f;
	if (fEntityY <= fGroundHeight + CBody::GetEpsilonError()) {
		m_oBody.m_oSpeed.m_x = 0;
		m_oBody.m_oSpeed.m_y = 0;
		m_oBody.m_oSpeed.m_z = 0;
		localPos.m_y = fGroundHeight + h / 2.f;
		worldPos.m_y = m_pScene->GetHeight(worldPos.m_x, worldPos.m_z) + h / 2.f;
		SetWorldPosition(worldPos);
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
		oParentWorld.GetInverse(oParentWorldInv);
		pChild->SetLocalMatrix(oParentWorldInv);
	}
	else if (  t = eSetChildToParentTM )
	{
		CMatrix oIdentity;
		pChild->SetLocalMatrix( oIdentity );
	}
	pChild->Link( pParentBone );
	CEntity* pChildEntity = static_cast< CEntity* >( pChild );
	IMesh* pMesh = dynamic_cast< IMesh* >( pChild->GetRessource() );
	if(pMesh)
		pChild->LocalTranslate( pMesh->GetOrgMaxPosition() );
}

void CEntity::Link( CNode* pParent )
{
	CNode::Link( pParent );
	CScene* pScene = dynamic_cast< CScene* >(pParent);
	if (pScene)
		m_pScene = pScene;
	if( m_pEntityManager )
	{
		if(pScene && m_fBoundingSphereRadius > 0.f )
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
	pInitRoot->GetWorldMatrix(m0);
	pCurrentRoot->GetWorldMatrix(m1);
	CMatrix::GetPassage( m0, m1, mPassage );
	
	CMatrix oWorldInverse;
	m_oWorldMatrix.GetInverse(oWorldInverse);
	mPassage = oWorldInverse * mPassage;	

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
	m_bDrawBoundingBox = bDraw;
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
	if(m_sName == "")
		SetName(sName);
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
