#include "Entity.h"
#include "Scene.h"

// stl
#include <algorithm>

// engine
#include "IRenderer.h"
#include "IRessource.h"
#include "IShader.h"
#include "IGeometry.h"
#include "ICollisionManager.h"
#include "Interface.h"
#include "EntityManager.h"
#include "PlaneEntity.h"
#include "Bone.h"

// Utils
#include "Utils2/TimeManager.h"
#include "Utils2/RenderUtils.h"

using namespace std;

const float g_fMaxHeight = 80;

CEntity::CEntity(EEInterface& oInterface):
m_oRessourceManager(*static_cast<IRessourceManager*>(oInterface.GetPlugin("RessourceManager"))),
m_oRenderer(*static_cast<IRenderer*>(oInterface.GetPlugin("Renderer"))),
m_oGeometryManager(*static_cast<IGeometryManager*>(oInterface.GetPlugin("GeometryManager"))),
m_oCollisionManager(*static_cast<ICollisionManager*>(oInterface.GetPlugin("CollisionManager"))),
m_oInterface(oInterface),
m_pCurrentAnimation( NULL ),
m_pOrgSkeletonRoot( NULL ),
m_pSkeletonRoot( NULL ),
m_bHidden( false ),
m_pEntityRoot( NULL ),
m_bUsePositionKeys( false ),
m_eRenderType( IRenderer::eFill ),
m_pBoundingSphere( NULL ),
m_fBoundingSphereRadius( -1 ),
m_bDrawAnimationBoundingBox( false ),
m_pfnCollisionCallback( NULL ),
m_pBoundingGeometry(NULL),
m_pRessource(NULL),
m_fMaxStepHeight(g_fMaxHeight),
m_pCollisionMesh(NULL),
m_bDrawBoundingBox(false),
m_pScene(NULL),
m_pMesh(nullptr),
m_bEmptyEntity(false),
m_pBaseTexture(nullptr),
m_pCustomTexture(nullptr)
{
	m_pEntityManager = static_cast<CEntityManager*>(oInterface.GetPlugin("EntityManager"));
}

CEntity::CEntity(EEInterface& oInterface, const string& sFileName, bool bDuplicate ):
m_oInterface(oInterface),
m_oRessourceManager(*static_cast<IRessourceManager*>(oInterface.GetPlugin("RessourceManager"))),
m_oRenderer(*static_cast<IRenderer*>(oInterface.GetPlugin("Renderer"))),
m_oGeometryManager(*static_cast<IGeometryManager*>(oInterface.GetPlugin("GeometryManager"))),
m_oCollisionManager(*static_cast<ICollisionManager*>(oInterface.GetPlugin("CollisionManager"))),
m_pEntityManager(static_cast<CEntityManager*>(oInterface.GetPlugin("EntityManager"))),
m_pCurrentAnimation( NULL ),
m_pOrgSkeletonRoot( NULL ),
m_pSkeletonRoot( NULL ),
m_bHidden( false ),
m_pEntityRoot( NULL ),
m_bUsePositionKeys( false ),
m_eRenderType( IRenderer::eFill ),
m_pBoundingSphere( NULL ),
m_fBoundingSphereRadius( -1 ),
m_bDrawAnimationBoundingBox( false ),
m_pfnCollisionCallback( NULL ),
m_pCollisionMesh(NULL),
m_pRessource(NULL),
m_pBoundingGeometry(NULL),
m_fMaxStepHeight(g_fMaxHeight),
m_bDrawBoundingBox(false),
m_pScene(NULL),
m_pMesh(nullptr),
m_bEmptyEntity(false),
m_pBaseTexture(nullptr),
m_pCustomTexture(nullptr)
{
	if( sFileName.size() > 0 )
	{
		SetRessource( sFileName);
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


void CEntity::SetRessource( string sFileName, bool bDuplicate )
{
	int nPos = static_cast< int >( sFileName.find_last_of( "." ) );
	string sExt = sFileName.substr( nPos + 1, 3 );
	string sExtLower = sExt;
	transform( sExt.begin(), sExt.end(), sExtLower.begin(), tolower );
	if ( sExtLower == "ale" )
		bDuplicate = true;
	IAnimatableMesh* pAMesh = dynamic_cast< IAnimatableMesh* >( m_oRessourceManager.GetRessource( sFileName, bDuplicate ) );
	if( pAMesh )
	{
		if( pAMesh->GetMeshCount() > 0 )
		{
			m_pRessource = pAMesh->GetMesh( 0 );
			m_pMesh = dynamic_cast< IMesh* >(m_pRessource);
			m_pBaseTexture = m_pMesh->GetTexture(0);
			m_pRessource->GetName( m_sName );
			m_pOrgSkeletonRoot = dynamic_cast<CBone*>(pAMesh->GetSkeleton());
			if (m_pOrgSkeletonRoot) {
				m_pSkeletonRoot = dynamic_cast<CBone*>(m_pOrgSkeletonRoot->DuplicateHierarchy());
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
					CEntity* pEntity = dynamic_cast< CEntity* >( m_pEntityManager->CreateEmptyEntity( sName ) );
					pEntity->SetMesh( pMesh );
					if (pMesh == m_pRessource)
						m_bEmptyEntity = true;
					
					pEntity->SetName( sName );
					LinkEntityToBone( pEntity, pParentBone );
				}
			}
			ILoaderManager* pLoaderManager = static_cast<ILoaderManager*>(m_oInterface.GetPlugin("LoaderManager"));
			string bboxFileName = sFileName.substr(0, sFileName.find(".")) + ".bbox";
			ILoader::CAnimationBBoxInfos bboxInfos;
			try {
				pLoaderManager->Load(bboxFileName, bboxInfos);
				m_oKeyBoundingBoxes = bboxInfos.mKeyBoundingBoxes;
				map<string, map<int, IBox*> >::iterator itBoxes = m_oKeyBoundingBoxes.find("stand-normal");
				if (itBoxes == m_oKeyBoundingBoxes.end()) {
					ostringstream oss;
					oss << "Erreur : l'entite '" << sFileName << "' ne possede pas de bounding box pour 'stand-normal'";
					exception e(oss.str().c_str());
					throw e;
				}
				m_pBoundingGeometry = itBoxes->second.begin()->second;
			}
			catch (CFileNotFoundException& e) {
				m_pBoundingGeometry = m_pMesh->GetBBox();
			}
			CreateAndLinkCollisionChildren(sFileName);
		}
	}
	else
	{
		CEException e( "Nous vous autorisons � fouetter le d�veloppeur" );
		throw e;
	}

	for (int i = 0; i < m_vEntityCallback.size(); i++) {
		m_vEntityCallback[i](nullptr, IEventDispatcher::TEntityEvent::T_LOAD_RESSOURCE, this);
	}
}

void CEntity::SetDiffuseTexture(string sFileName)
{
	m_pCustomTexture = m_oRessourceManager.CreateTexture2D(sFileName, true);
}

void CEntity::CreateAndLinkCollisionChildren(string sFileName)
{
	int dotPos = (int)sFileName.find('.');
	string sPrefix = sFileName.substr(0, dotPos);
	string sCollisionFileName = sPrefix + ".col";
	try {
		m_pCollisionMesh = dynamic_cast<ICollisionMesh*>(m_oRessourceManager.GetRessource(sCollisionFileName, false));
		for (int i = 0; i < m_pCollisionMesh->GetGeometryCount(); i++) {
			IGeometry* pGeometry = m_pCollisionMesh->GetGeometry(i);
			ostringstream oss;
			oss << sPrefix << "_CollisionPrimitive" << i;
			CCollisionEntity* pChild = m_pEntityManager->CreateCollisionEntity(oss.str());
			pChild->SetLocalMatrix(pGeometry->GetTM());
			pChild->ForceAssignBoundingGeometry(pGeometry);
			pChild->m_fBoundingSphereRadius = pGeometry->ComputeBoundingSphereRadius();
			pChild->Link(this);
			pChild->SetEntityName(oss.str());
		}
	}
	catch (CFileNotFoundException& e) {
		e = e;
	}
}

void CEntity::UpdateCollision()
{
	m_oBody.Update();
	if( m_oBody.m_fWeight > 0.f )
	{
		if( m_pScene )
		{

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
			const float margin = -20.;
			float fGroundHeight = m_pScene->GetGroundHeight(x, z) + m_pScene->GetGroundMargin();
			float fEntityZ = m_oLocalMatrix.m_13 + m_pBoundingGeometry->GetBase().m_y + m_oBody.m_oSpeed.m_y * (float)nDelta / 1000.f;
			if( fEntityZ > fGroundHeight + CBody::GetEpsilonError() )
			{
				if( nDelta != 0 )
				{
					CVector vTranslation = m_oBody.m_oSpeed * ((float)nDelta / 1000.f);
					LocalTranslate( vTranslation );
				}
			}
			else
			{
				m_oBody.m_oSpeed.m_x = 0;
				m_oBody.m_oSpeed.m_y = 0;
				m_oBody.m_oSpeed.m_z = 0;
				if( fEntityZ < fGroundHeight + CBody::GetEpsilonError() )
					SetLocalPosition( m_oLocalMatrix.m_03, fGroundHeight - m_pBoundingGeometry->GetBase().m_y, m_oLocalMatrix.m_23  );
			}
		}
	}
}

bool CEntity::TestCollision(INode* pEntity)
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

float CEntity::GetGroundHeight(float x, float z)
{
	float fGroundHeight = 0.f;
	if (m_pParent) {
		CVector localPos(x, 0, z);
		CVector worldPosition = m_oWorldMatrix * localPos;
		fGroundHeight = m_pParent->GetGroundHeight(worldPosition.m_x, worldPosition.m_z) - GetY();
	}
	return fGroundHeight;
}

void CEntity::GetEntitiesCollision(vector<INode*>& entities)
{
	for (unsigned int i = 0; i < m_pParent->GetChildCount(); i++) {
		INode* pEntity = m_pParent->GetChild(i);
		if (!pEntity || pEntity == this)
			continue;

		if (TestCollision(pEntity))
			entities.push_back(pEntity);
	}
}

float CEntity::GetBoundingSphereDistance(INode* pEntity)
{
	CVector oThisPosition, oEntityWorldPosition;
	GetWorldPosition(oThisPosition);
	pEntity->GetWorldPosition(oEntityWorldPosition);
	float fDistance = (oThisPosition - oEntityWorldPosition).Norm() - (m_fBoundingSphereRadius) - (pEntity->GetBoundingSphereRadius());
	return fDistance;
}

void CEntity::SendBonesToShader()
{
	if (m_pOrgSkeletonRoot)
	{
		vector< CMatrix > vBoneMatrix;
		GetBonesMatrix(m_pOrgSkeletonRoot, m_pSkeletonRoot, vBoneMatrix);
		SetNewBonesMatrixArray(vBoneMatrix);
	}
}

void CEntity::UpdateRessource()
{
	if (!m_bHidden)
	{
		if (m_pMesh)
		{
			m_pMesh->SetRenderingType(m_eRenderType);
			m_pMesh->DrawAnimationBoundingBox(m_bDrawAnimationBoundingBox);
		}
		if (m_pRessource) {
			if (m_pMesh && m_pCustomTexture)
				m_pMesh->SetTexture(m_pCustomTexture);
			m_pRessource->Update();
			if (m_pMesh && m_pCustomTexture)
				m_pMesh->SetTexture(m_pBaseTexture);
		}
		if (m_pMesh)
			m_pMesh->SetRenderingType(IRenderer::eFill);
	}
}

void CEntity::Update()
{
	UpdateCollision();

	if( m_pCurrentAnimation )
		m_pCurrentAnimation->Update();

	CNode::Update();
	SendBonesToShader();

	m_oWorldMatrix *= m_oScaleMatrix;
	m_oRenderer.SetModelMatrix( m_oWorldMatrix );	
	UpdateRessource();

	if (m_bDrawBoundingBox && m_pBoundingGeometry)
		UpdateBoundingBox();

	DispatchEntityEvent();
}

void CEntity::DispatchEntityEvent()
{
	for (int i = 0; i < m_vEntityCallback.size(); i++) {
		m_vEntityCallback[i](nullptr, IEventDispatcher::TEntityEvent::T_UPDATE, this);
	}
}

void CEntity::UpdateBoundingBox()
{
	IBox* pBBox = dynamic_cast<IBox*>(m_pBoundingGeometry);
	if(pBBox)
		CRenderUtils::DrawBox(pBBox->GetMinPoint(), pBBox->GetDimension(), m_oRenderer);
}

float CEntity::GetHeight()
{
	return m_pBoundingGeometry->GetHeight();
}

void CEntity::LocalTranslate(float dx, float dy, float dz)
{
	if ( (m_fBoundingSphereRadius > 0) && (GetWeight() > 0.f) ) {
		float fStepHeight = dy != 0 ? 50.f : 0.f;
		bool bCollision = false;
		CVector oThisPos, oEntityPos;
		GetWorldPosition(oThisPos);

		INode* pEntity = NULL;
		vector<INode*> entities;
		GetEntitiesCollision(entities);

		CMatrix oTemp = m_oLocalMatrix;	
		if (!entities.empty())
		{
			for (int i = 0; i < entities.size(); i++) {
				pEntity = entities[i];
				pEntity->GetWorldPosition(oEntityPos);
				float fCurrentDistance = (oThisPos - oEntityPos).Norm();
				CNode::LocalTranslate(dx, dy, dz);
				CNode::UpdateWorldMatrix();
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
			CNode::UpdateWorldMatrix();
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
			m_pfnCollisionCallback(this, entities);
		}
	}
	else
		CNode::LocalTranslate(dx, dy, dz);
}

bool CEntity::ManageBoxCollision(vector<INode*>& vCollideEntities, float dx, float dy, float dz, const CMatrix& oBackupMatrix)
{
	INode* pCollideEntity = vCollideEntities[0];
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
			UpdateWorldMatrix();
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
	float fGroundHeight = m_pParent->GetGroundHeight(worldPos.m_x, worldPos.m_z) + m_pScene->GetGroundMargin();
	float fEntityY = localPos.m_y - h / 2.f;
	if (fEntityY <= fGroundHeight + CBody::GetEpsilonError()) {
		m_oBody.m_oSpeed.m_x = 0;
		m_oBody.m_oSpeed.m_y = 0;
		m_oBody.m_oSpeed.m_z = 0;
		localPos.m_y = fGroundHeight + h / 2.f;
		worldPos.m_y = m_pScene->GetGroundHeight(worldPos.m_x, worldPos.m_z) + h / 2.f;
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
	IMesh* pMesh = dynamic_cast< IMesh* >( pChild->GetRessource() );
	if(pMesh)
		pChild->LocalTranslate( pMesh->GetOrgMaxPosition().m_x, pMesh->GetOrgMaxPosition().m_y, pMesh->GetOrgMaxPosition().m_z);
}

void CEntity::LinkDummyParentToDummyEntity(IEntity* pEntity, string sDummyName)
{
	IBone* pEntityDummy = pEntity->GetSkeletonRoot()->GetChildBoneByName(sDummyName);
	if (!pEntityDummy) {
		throw CNodeNotFoundException(sDummyName);
	}
	CMatrix id;
	if (!m_pSkeletonRoot) {
		throw CNoDummyRootException("");
	}
	m_pSkeletonRoot->SetLocalMatrix(id);
	m_pSkeletonRoot->Link(pEntityDummy);
}

void CEntity::Link( INode* pParent )
{
	if (m_bEmptyEntity) {
		IBone* pDummy = dynamic_cast<IBone*>(GetChild(0));
		pDummy->Link(pParent);
		int id = m_pEntityManager->GetEntityID(this);
		IEntityManager* pEntityManager = m_pEntityManager;
		string sEntityName = m_sEntityName;
		CEntity* pDummyChildEntity = dynamic_cast<CEntity*>(pDummy->GetChild(0));
		if (!pDummyChildEntity) {
			ostringstream oss;
			string sDummyName;
			pDummy->GetName(sDummyName);
			oss << "Dummy child not found for dummy \"" << sDummyName << "\"";
			throw CEException(oss.str());
		}
		pDummyChildEntity->m_pScene = m_pScene;
		pDummyChildEntity->m_pSkeletonRoot = m_pSkeletonRoot;
		pEntityManager->DestroyEntity(this);
		pEntityManager->AddEntity(pDummyChildEntity, sEntityName, id);
		return;
	}
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

void CEntity::GetBonesMatrix( INode* pInitRoot, INode* pCurrentRoot, vector< CMatrix >& vMatrix )
{
	// m0 = base du node dans sa position initiale, m0i = inverse de m0
	// m1 = base du node dans sa position actuelle
	// m2 = matrice de passage de m0 � m1 (m2 = inv(m0)*m1)
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
	m_pRessource->GetShader()->SendUniformMatrix4Array( "matBones", vMatBones, true );
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

void CEntity::SetShader(IShader* pShader)
{
	m_pRessource->SetShader(pShader);
}

void CEntity::CenterToworld()
{
#if 0
	IBox* pBbox = m_oGeometryManager.CreateBox();
	static_cast<IMesh*>(m_pRessource)->GetBbox(*pBbox);
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

void CEntity::AddAnimation(string sAnimationFile)
{
	if (m_pSkeletonRoot)
	{
		IAnimation* pAnimation = static_cast<IAnimation*>(m_oRessourceManager.GetRessource("/Animations/" + sAnimationFile, true));
		m_mAnimation[sAnimationFile] = pAnimation;
		IMesh* pMesh = static_cast<IMesh*>(m_pRessource);
		pAnimation->SetSkeleton(m_pSkeletonRoot);
	}
	else
	{
		ostringstream oss;
		oss << "CEntity::AddAnimation() : Erreur : l'entit� num�ro " << m_nID << " n'est pas animable.";
		CEException e(oss.str());
		throw e;
	}
}

void CEntity::SetCurrentAnimation(std::string sAnimation)
{
	m_sCurrentAnimation = sAnimation;
	m_pCurrentAnimation = m_mAnimation[sAnimation];
	if (m_bUsePositionKeys)
		m_pCurrentAnimation->AddCallback(OnAnimationCallback, this);
}

bool CEntity::HasAnimation(string sAnimationName)
{
	map< string, IAnimation* >::iterator itAnim = m_mAnimation.find(sAnimationName);
	return itAnim != m_mAnimation.end();
}

IBone* CEntity::GetSkeletonRoot()
{
	return m_pSkeletonRoot;
}


void CEntity::GetEntityInfos(ILoader::CObjectInfos*& pInfos)
{
	if (!pInfos)
		pInfos = new ILoader::CEntityInfos;
	ILoader::CEntityInfos* pEntityInfos = dynamic_cast<ILoader::CEntityInfos*>(pInfos);
	if (pEntityInfos) {
		GetEntityName(pEntityInfos->m_sObjectName);
		pEntityInfos->m_fWeight = GetWeight();
		for (unsigned int iChild = 0; iChild < GetChildCount(); iChild++) {
			CEntity* pChild = dynamic_cast<CEntity*>(GetChild(iChild));
			if (pChild) {
				ILoader::CObjectInfos* pChildInfos = nullptr;
				pChild->GetEntityInfos(pChildInfos);
				if (pChildInfos)
					pEntityInfos->m_vSubEntityInfos.push_back(pChildInfos);
			}
		}
	}
	CMatrix oMat;
	GetLocalMatrix(oMat);
	pInfos->m_oXForm = oMat;
	GetParent()->GetName(pInfos->m_sParentName);
	IBone* pParentBone = dynamic_cast<IBone*>(GetParent());
	if (pParentBone) {
		pInfos->m_nParentBoneID = pParentBone->GetID();
		if (pParentBone->GetID() == 0) {
			IBone* pGrandParentBone = dynamic_cast<IBone*>(pParentBone->GetParent());
			if (pGrandParentBone) {
				string sParentName, sGrandParentName;
				pParentBone->GetName(sParentName);
				pGrandParentBone->GetName(sGrandParentName);
				string sDummyPrefix = "Dummy";
				string sBodyDummyPrefix = "BodyDummy";
				if ((sParentName.substr(0, sDummyPrefix.size()) == sDummyPrefix) &&
					(sGrandParentName.substr(0, sBodyDummyPrefix.size()) == sBodyDummyPrefix)) {
					pEntityInfos->m_nGrandParentDummyRootID = pGrandParentBone->GetID();
				}
			}
		}
	}
	if (pInfos->m_sObjectName.empty()) {
		string sName;
		GetName(sName);
		pInfos->m_sObjectName = sName;
	}
	if(m_pRessource) {
		m_pRessource->GetFileName(pInfos->m_sRessourceFileName);
		m_pRessource->GetName(pInfos->m_sRessourceName);
	}
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
			m_pBoundingSphere = m_pEntityManager->CreateEntity("sphere.bme", "");
		if( pSkeleton )
			LinkEntityToBone( m_pBoundingSphere, pSkeleton );
		else
			m_pBoundingSphere->Link( this );
		m_pBoundingSphere->SetScaleFactor( m_fBoundingSphereRadius, m_fBoundingSphereRadius, m_fBoundingSphereRadius );
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
			m_mBonesBoundingSphere[ nID ] = m_pEntityManager->CreateEntity("sphere.bme", "");
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
	CEException e( "La m�thode \"Goto\" n'est pas encore impl�ment�e pour les entit� g�n�riques. Veuillez l'utiliser uniquement pour les entit� de type \"IAEntity\"" );
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
		for (int i = 0; i < pMesh->GetMaterialCount(); i++) {
			pMesh->GetMaterial(i)->SetAdditionalColor(r, g, b, a);
		}
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

void CEntity::AbonneToEntityEvent(IEventDispatcher::TEntityCallback callback)
{
	m_vEntityCallback.push_back(callback);
}

void CEntity::DeabonneToEntityEvent(IEventDispatcher::TEntityCallback callback)
{
	vector<IEventDispatcher::TEntityCallback>::iterator it = std::find(m_vEntityCallback.begin(), m_vEntityCallback.end(), callback);
	if (it != m_vEntityCallback.end()) {
		m_vEntityCallback.erase(it);
	}
}

void CEntity::GetSkeletonEntities(CBone* pRoot, vector< CEntity* >& vEntity, string sFileFilter)
{
	for (unsigned int iChild = 0; iChild < pRoot->GetChildCount(); iChild++)
	{
		CEntity* pEntity = dynamic_cast< CEntity* >(pRoot->GetChild(iChild));
		if (pEntity)
		{
			string sFileName;
			pEntity->GetRessource()->GetFileName(sFileName);
			if (sFileFilter != sFileName)
				vEntity.push_back(pEntity);
		}
		else
			GetSkeletonEntities(dynamic_cast<CBone*>(pRoot->GetChild(iChild)), vEntity, sFileFilter);
	}
}