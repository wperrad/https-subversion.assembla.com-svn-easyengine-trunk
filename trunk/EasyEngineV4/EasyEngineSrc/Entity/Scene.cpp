#include "Scene.h"
#include "IRenderer.h"
#include "FreeCamera.h"
#include "ICameraManager.h"
#include "IRessource.h"
#include "Shape.h"
#include "IEntity.h"
#include "LightEntity.h"
#include "ICollisionManager.h"
#include "ISystems.h"
#include "Utils2/StringUtils.h"
#include "Utils2/TimeManager.h"
#include "IPathFinder.h"
#include "EntityManager.h"
#include "MobileEntity.h"
#include "MapEntity.h"
#include "SphereEntity.h"

using namespace std;

CScene::Desc::Desc( IRessourceManager& oRessourceManager, IRenderer& oRenderer, IEntityManager* pEntityManager, 
	ICamera* pCamera, ICameraManager& oCameraManager, ILoaderManager& oLoaderManager, 
	ICollisionManager& oCollisionManager, IGeometryManager& oGeometryManager, IPathFinder& oPathFinder ):
m_oRessourceManager( oRessourceManager ),
m_oRenderer( oRenderer ),
m_pCamera( pCamera ),
m_oCameraManager( oCameraManager ),
m_oLoaderManager( oLoaderManager ),
m_oCollisionManager( oCollisionManager ),
m_oGeometryManager( oGeometryManager ),
m_oPathFinder(oPathFinder)
{
	m_pEntityManager = pEntityManager;
}

CScene::CScene( const Desc& oDesc ):
CEntity( oDesc.m_sFileName, oDesc.m_oRessourceManager, oDesc.m_oRenderer, NULL, oDesc.m_oGeometryManager, oDesc.m_oCollisionManager ),
m_pCamera( oDesc.m_pCamera ),
m_oCameraManager( oDesc.m_oCameraManager ),
m_oLoaderManager( oDesc.m_oLoaderManager ),
m_oCollisionManager( oDesc.m_oCollisionManager ),
m_oPathFinder(oDesc.m_oPathFinder),
m_nHeightMapID( -1 ),
m_bCollisionMapCreated(true),
m_bHeightMapCreated(true),
m_pCollisionGrid(NULL),
m_sMapFirstPassShaderName("mapFirstPass"),
m_sMapSecondPassShaderName("mapSecondPass2D"),
m_pPlayer(NULL),
m_bDisplayMap(false),
m_fGroundMargin(-10.f)
{
	SetName( "Scene" );
	SetEntityName("Scene");
	m_pRessource = NULL;
	m_pEntityManager = static_cast<CEntityManager*>(oDesc.m_pEntityManager);


	ICamera* pMapCamera = m_oCameraManager.CreateCamera(ICameraManager::T_MAP_CAMERA, 40.f, *m_pEntityManager);
	pMapCamera->Link(this);

	m_pMapCamera = m_oCameraManager.GetCameraFromType(ICameraManager::T_MAP_CAMERA);
	m_pMapCamera->SetWorldPosition(0, 40000, 0);
	m_pMapCamera->Pitch(-90);

	m_pMapTexture = CreateMapTexture();

	m_pPlayerMapSphere = static_cast<CEntity*>(m_pEntityManager->CreateEntity("playerPointer.bme", "", m_oRenderer));
	m_pPlayerMapSphere->SetShader(m_oRenderer.GetShader(m_sMapFirstPassShaderName));

	m_StringToAnimation["run"] = IEntity::eRun;
}

CScene::~CScene()
{
}

ITexture* CScene::CreateMapTexture()
{
	int nMapWidth, nMapHeight;
	m_oRenderer.GetResolution(nMapWidth, nMapHeight);
	ITexture* pMapTexture = m_oRessourceManager.CreateRenderTexture(nMapWidth, nMapHeight, m_sMapSecondPassShaderName);
	return pMapTexture;
}

ITexture* CScene::GetMapTexture()
{
	return m_pMapTexture;
}

void CScene::SetRessource( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, bool bDuplicate )
{
	CEntity::SetRessource( sFileName, oRessourceManager, oRenderer, bDuplicate );
	int nDotPos = (int)sFileName.find('.');
	m_sHMFileName = string("hm_") + sFileName.substr(0, nDotPos) + ".bmp";
	IMesh* pMesh = static_cast< IMesh* >(m_pRessource);

	try
	{
		m_nHeightMapID = m_oCollisionManager.LoadHeightMap(m_sHMFileName, pMesh );
	}
	catch( CFileNotFoundException& )
	{
		m_bHeightMapCreated = false;
	}
	
	nDotPos = (int)sFileName.find('.');
	m_sCollisionFileName = string("collision_") + sFileName.substr(0, nDotPos) + ".bmp";
	try{
		m_oCollisionManager.LoadCollisionMap(m_sCollisionFileName, this);
		m_bCollisionMapCreated = true;
		CreateCollisionGrid();
	}
	catch (CFileNotFoundException& e) {
		m_bCollisionMapCreated = false;
	}	
}

IGrid* CScene::GetCollisionGrid()
{
	return m_pCollisionGrid;
}

void CScene::CreateCollisionMap()
{
	IMesh* pGroundMesh = dynamic_cast<IMesh*>(GetRessource());
	if(pGroundMesh){
		vector<IEntity*> collides;
		IEntity* pCollideEntity = m_pEntityManager->GetFirstCollideEntity();
		while (pCollideEntity) {
			collides.push_back(pCollideEntity);
			pCollideEntity = m_pEntityManager->GetNextCollideEntity();
		}
		ILoader::CTextureInfos ti;
		m_oCollisionManager.CreateCollisionMap(ti, collides, this, IRenderer::T_BGR);
		ti.m_ePixelFormat = ILoader::eBGR;
		string sGroundName;
		pGroundMesh->GetName(sGroundName);
		m_oLoaderManager.Export(m_sCollisionFileName, ti);
		m_oCollisionManager.LoadCollisionMap(m_sCollisionFileName, this);
		m_bCollisionMapCreated = true;
	}
	else {
		CEException e("Erreur : La sc�ne ne poss�de pas de map");
		throw e;
	}
}

void CScene::CreateHeightMap()
{
	IMesh* pMesh = static_cast< IMesh* >(m_pRessource);
	ILoader::CTextureInfos ti;
	m_oCollisionManager.CreateHeightMap(pMesh, ti, IRenderer::T_BGR);
	ti.m_ePixelFormat = ILoader::eBGR;	
	m_oLoaderManager.Export(m_sHMFileName, ti);
	m_nHeightMapID = m_oCollisionManager.LoadHeightMap(m_sHMFileName, pMesh);
	m_bHeightMapCreated = true;
}

void CScene::CreateCollisionGrid()
{
	int rowCount, columnCount;
	m_oCollisionManager.ComputeRowAndColumnCount(rowCount, columnCount);
	m_pCollisionGrid = m_oPathFinder.CreateGrid(rowCount, columnCount);

	for (int iRow = 0; iRow < rowCount; iRow++) {
		for (int iColumn = 0; iColumn < columnCount; iColumn++) {
			bool obstacle = m_oCollisionManager.TestCellObstacle(iRow, iColumn);
			if (obstacle)
				m_pCollisionGrid->AddObstacle(iRow, iColumn);
		}
	}
}

IEntity* CScene::Merge( string sRessourceName, string sEntityType, float x, float y, float z )
{
	IEntity* pEntity = m_pEntityManager->CreateEntity( sRessourceName, sEntityType, m_oRenderer );
	pEntity->Link( this );
	pEntity->SetLocalPosition( x, y, z );
	return pEntity;
}

IEntity* CScene::Merge( string sRessourceName, string sEntityType, CMatrix& oXForm )
{
	IEntity* pEntity = m_pEntityManager->CreateEntity( sRessourceName, sEntityType, m_oRenderer );
	pEntity->SetLocalMatrix( oXForm );
	pEntity->Link( this );
	return pEntity;
}

void CScene::Update()
{
	static int counter = 0;
	if (m_bHeightMapCreated && !m_bCollisionMapCreated && counter++ == 10) {
		CreateCollisionMap();
		CreateCollisionGrid();
	}	
	
	if (!m_bHeightMapCreated && counter++ == 20 ){
		CreateHeightMap();
	}
	if (m_vMapEntities.size() == 0) {
		OnChangeSector();
	}
	CTimeManager::Instance()->Update();
	RenderScene();

	if (m_bDisplayMap) {
		RenderMap();
		m_oRenderer.SetCurrentFBO(0);
	}
	
}

void CScene::DisplayMap(bool display)
{
	m_bDisplayMap = display;
}

void CScene::SetGroundMargin(float margin)
{
	m_fGroundMargin = margin;
}

float CScene::GetGroundMargin()
{
	return m_fGroundMargin;
}

void CScene::GetOriginalSceneFileName(string& sFileName)
{
	if (!m_sOriginalSceneFileName.empty())
		sFileName = m_sOriginalSceneFileName;
	else if(m_pRessource)
		m_pRessource->GetFileName(sFileName);
}

void CScene::SetOriginalSceneFileName(string sFileName)
{
	m_sOriginalSceneFileName = sFileName;
}

void  CScene::RenderScene()
{
	if (m_oCameraManager.GetActiveCamera()) {
		CMatrix oCamMatrix;
		m_oCameraManager.GetActiveCamera()->Update();
		m_oCameraManager.GetActiveCamera()->GetWorldMatrix(oCamMatrix);
		m_oRenderer.SetCameraMatrix(oCamMatrix);
	}
	CNode::Update();
	m_oRenderer.SetModelMatrix(m_oWorldMatrix);
	if (m_pRessource)
		m_pRessource->Update();
}

void CScene::RenderMap()
{
	// first pass
	m_oRenderer.SetCurrentFBO(m_pMapTexture->GetFrameBufferObjectId());
	DisplayEntities(m_vMapEntities);
}

void CScene::CollectMapEntities(vector<IEntity*>& entities)
{
	for (int i = 0; i < GetChildCount(); i++) {
		CEntity* pEntity = dynamic_cast<CEntity*>(GetChild(i));
		if (pEntity) {
			if (pEntity != this) {
				CMobileEntity* pMobile = dynamic_cast<CMobileEntity*>(pEntity);
				if (!pMobile) {
					CLightEntity* pLightEntity = dynamic_cast<CLightEntity*>(pEntity);
					if (!pLightEntity) {
						CMapEntity* pMapEntity = dynamic_cast<CMapEntity*>(pEntity);
						if(!pMapEntity)
							entities.push_back(pEntity);
					}
				}
				else {
					IPlayer* pPlayer = dynamic_cast<IPlayer*>(pMobile);
					if (pPlayer) {
						m_pPlayer = pEntity;
						entities.push_back(m_pPlayer);
					}
				}
			}
		}
	}
}

void CScene::OnChangeSector()
{
	CollectMapEntities(m_vMapEntities);
}

void CScene::UpdateMapEntities()
{
	CollectMapEntities(m_vMapEntities);
}

void CScene::DisplayEntities(vector<IEntity*>& entities)
{
	ICamera* pActiveCamera = m_oCameraManager.GetActiveCamera();
	CMatrix oCamMatrix;
	m_pMapCamera->SetLocalPosition(m_pPlayer->GetX(), m_pMapCamera->GetY(), m_pPlayer->GetZ());
	m_pMapCamera->Update();
	m_pMapCamera->GetWorldMatrix(oCamMatrix);
	CMatrix oBackupInvCameraMatrix;
	m_oRenderer.GetInvCameraMatrix(oBackupInvCameraMatrix);
	m_oRenderer.SetCameraMatrix(oCamMatrix);
	m_oCameraManager.SetActiveCamera(m_pMapCamera);
	m_oRenderer.ClearFrameBuffer();
	IShader* pBackupShader = NULL;
	IShader* pFirstPassShader = m_oRenderer.GetShader(m_sMapFirstPassShaderName);

	CMatrix m;
	m_oRenderer.SetModelMatrix(m);
	IMesh* pGround = static_cast<IMesh*>(GetRessource());
	pBackupShader = pGround->GetShader();
	pGround->SetShader(pFirstPassShader);
	pGround->Update();
	pGround->SetShader(pBackupShader);

	for (int i = 0; i < entities.size(); i++) {
		CEntity* pEntity = static_cast<CEntity*>(entities[i]);
		pBackupShader = pEntity->GetRessource()->GetShader();
		pEntity->SetShader(pFirstPassShader);
		m_oRenderer.SetModelMatrix(pEntity->GetWorldMatrix());
		pEntity->UpdateRessource();
		pEntity->SetShader(pBackupShader);
	}
	
	m_pPlayerMapSphere->SetLocalMatrix(m_pPlayer->GetWorldMatrix());
	m_pPlayerMapSphere->Update();

	m_oCameraManager.SetActiveCamera(pActiveCamera);
	m_oRenderer.SetInvCameraMatrix(oBackupInvCameraMatrix);
}

void CScene::GetSkeletonEntities( CNode* pRoot, vector< IEntity* >& vEntity, string sFileFilter )
{
	for( unsigned int iChild = 0; iChild < pRoot->GetChildCount(); iChild++ )
	{
		IEntity* pEntity = dynamic_cast< IEntity* >( pRoot->GetChild( iChild ) );
		if( pEntity )
		{
			string sFileName;
			pEntity->GetRessource()->GetFileName( sFileName );
			if( sFileFilter != sFileName )
				vEntity.push_back( pEntity );
		}
		else
			GetSkeletonEntities( pRoot->GetChild( iChild ), vEntity, sFileFilter );
	}
}

ILoader::CSceneObjInfos* CScene::GetEntityInfos( IEntity* pEntity )
{
	ILoader::CSceneObjInfos* pInfos = NULL;
	CLightEntity* pLe = dynamic_cast< CLightEntity* >( pEntity );
	if( pLe )
	{
		pInfos = new ILoader::CLightEntityInfos;
		ILoader::CLightEntityInfos* pLightInfos = static_cast< ILoader::CLightEntityInfos* >( pInfos );
		pLightInfos->m_fIntensity = pLe->GetIntensity();
		pLightInfos->m_oColor = pLe->GetColor();
		ILoader::CLightInfos::TLight type;
		switch( pLe->GetType() )
		{
		case IRessource::DIRECTIONAL:
			type = ILoader::CLightInfos::eDirectionnelle;
			break;
		case IRessource::OMNI:
			type = ILoader::CLightInfos::eOmni;
			break;
		case IRessource::SPOT:
			type = ILoader::CLightInfos::eTarget;
			break;
		}
		pLightInfos->m_eType = type;
	}
	else
	{
		CShape* pRepere = dynamic_cast< CShape* >( pEntity );
		if( pRepere )
			return NULL;
		else {
			ICamera* pCamera = dynamic_cast<ICamera*>(pEntity);
			if (pCamera) {
				return NULL;
			} else {
				pInfos = new ILoader::CEntityInfos;
				ILoader::CEntityInfos* pEntityInfos = static_cast<ILoader::CEntityInfos*>(pInfos);
				IAnimation* pAnimation = pEntity->GetCurrentAnimation();
				if (pAnimation) {
					pAnimation->GetFileName(pEntityInfos->m_sAnimationFileName);
					CMobileEntity* pMobileEntity = dynamic_cast<CMobileEntity*>(pEntity);
					for (map<string, IEntity::TAnimation>::iterator it = m_StringToAnimation.begin(); it != m_StringToAnimation.end(); it++) {
						float as = pMobileEntity->GetAnimationSpeed(it->second);
						pEntityInfos->m_mAnimationSpeed[it->first] = as;
					}					
				}
				pEntityInfos->m_fWeight = pEntity->GetWeight();
				pEntity->GetTypeName(pEntityInfos->m_sTypeName);
				IBone* pSkeleton = pEntity->GetSkeletonRoot();
				if (pSkeleton)
				{
					vector< IEntity* > vSubEntity;
					string sRessourceFileName;
					pEntity->GetRessource()->GetFileName(sRessourceFileName);
					GetSkeletonEntities(pSkeleton, vSubEntity, sRessourceFileName);
					for (unsigned int iSubEntity = 0; iSubEntity < vSubEntity.size(); iSubEntity++)
					{
						ILoader::CEntityInfos* pSubEntityInfo = dynamic_cast<ILoader::CEntityInfos*>(GetEntityInfos(vSubEntity[iSubEntity]));
						if (pSubEntityInfo)
							pEntityInfos->m_vSubEntityInfos.push_back(pSubEntityInfo);
					}
				}
			}
		}
	}
	IRessource* pRessource = pEntity->GetRessource();
	if( pRessource )
		pRessource->GetFileName( pInfos->m_sRessourceFileName );
	CMatrix oMat;
	pEntity->GetLocalMatrix( oMat );
	pInfos->m_oXForm = oMat;
	pEntity->GetParent()->GetName( pInfos->m_sParentName  );
	IBone* pParentBone = dynamic_cast< IBone* >( pEntity->GetParent() );
	if( pParentBone )
		pInfos->m_nParentBoneID = pParentBone->GetID();
	string sName;
	pEntity->GetName( sName );
	pInfos->m_sObjectName = sName;
	pEntity->GetRessource()->GetFileName( pInfos->m_sRessourceFileName );
	pEntity->GetRessource()->GetName( pInfos->m_sRessourceName );
	return pInfos;
}

void CScene::GetInfos( ILoader::CSceneInfos& si )
{
	m_pRessource->GetFileName( si.m_sSceneFileName );
	si.m_sOriginalSceneFileName = m_sOriginalSceneFileName;
	GetName( si.m_sName );
	m_oRenderer.GetBackgroundColor(si.m_oBackgroundColor);
	for( unsigned int i= 0; i < m_vChild.size(); i++ )
	{		
		IEntity* pEntity = static_cast< IEntity* >( m_vChild[ i ] );		
		ILoader::CSceneObjInfos* pInfos = GetEntityInfos( pEntity );
		if( pInfos )
			si.m_vObject.push_back( pInfos );
	}
}

void CScene::LoadSceneObject( const ILoader::CSceneObjInfos* pSceneObjInfos, IEntity* pParent )
{
	string sRessourceFileName = pSceneObjInfos->m_sRessourceFileName;
	CMatrix oXForm = pSceneObjInfos->m_oXForm;
	if( sRessourceFileName == "EE_Repere_19051978" )
	{
		IEntity* pRepere = m_pEntityManager->CreateRepere( m_oRenderer );
		pRepere->Link( this );
	}
	else
	{
		const ILoader::CLightEntityInfos* pLightEntityInfos = dynamic_cast< const ILoader::CLightEntityInfos* >( pSceneObjInfos );
		if( pLightEntityInfos )
		{
			if( sRessourceFileName.size() == 0 )
			{
				IRessource::TLight type;
				switch( pLightEntityInfos->m_eType )
				{
				case ILoader::CLightInfos::eDirectionnelle:
					type = IRessource::DIRECTIONAL;
					break;
				case ILoader::CLightInfos::eOmni:
					type = IRessource::OMNI;
					break;
				case ILoader::CLightInfos::eTarget:
					type = IRessource::SPOT;
					break;
				default:
					throw 1;
				}
				IEntity* pEntity = m_pEntityManager->CreateLightEntity( pLightEntityInfos->m_oColor, type, pLightEntityInfos->m_fIntensity );
				pEntity->SetLocalMatrix( oXForm );
				pEntity->Link( this );
			}
			else
				throw 1;
		}
		else
		{
			const ILoader::CEntityInfos* pEntityInfos = dynamic_cast< const ILoader::CEntityInfos* >( pSceneObjInfos );
			IEntity* pEntity = m_pEntityManager->CreateEntity( sRessourceFileName, pEntityInfos->m_sTypeName, m_oRenderer );
			pEntity->SetLocalMatrix( oXForm );

			if( pEntityInfos->m_nParentBoneID != -1 )
			{
				if( pParent->GetSkeletonRoot() )
				{
					IBone* pBone = dynamic_cast< IBone* >( pParent->GetSkeletonRoot()->GetChildBoneByID( pEntityInfos->m_nParentBoneID ) );
					if( pBone )
						pParent->LinkEntityToBone( pEntity, pBone );
					else
					{
						ostringstream oss;
						oss << "Erreur, ";
						CEException e( oss.str() );
						throw e;
					}
				}
			}
			else
				pEntity->Link( pParent );
			if( pEntity->GetSkeletonRoot() )
			{
				pEntity->AddAnimation( pEntityInfos->m_sAnimationFileName );
				pEntity->SetCurrentAnimation( pEntityInfos->m_sAnimationFileName );
				for (map<string, float>::const_iterator it = pEntityInfos->m_mAnimationSpeed.begin(); it != pEntityInfos->m_mAnimationSpeed.end(); it++) {
					pEntity->SetAnimationSpeed(m_StringToAnimation[it->first], it->second);
				}
				pEntity->GetCurrentAnimation()->Play( true );
			}
			pEntity->SetWeight( pEntityInfos->m_fWeight );
			for( unsigned int iChild = 0; iChild < pEntityInfos->m_vSubEntityInfos.size(); iChild++ )
				LoadSceneObject( pEntityInfos->m_vSubEntityInfos[ iChild ], pEntity );
		}
	}
}

void CScene::Load( const ILoader::CSceneInfos& si )
{
	m_oRenderer.SetBackgroundColor(si.m_oBackgroundColor);
	SetRessource( si.m_sSceneFileName, m_oRessourceManager, m_oRenderer );
	m_sOriginalSceneFileName = si.m_sOriginalSceneFileName;
	for( unsigned int i = 0; i < si.m_vObject.size(); i++ )
	{
		const ILoader::CSceneObjInfos* pSceneObjInfos = si.m_vObject.at( i );
		LoadSceneObject( pSceneObjInfos, this );
	}
	m_pEntityManager->SetPlayer( m_pEntityManager->GetPlayer() );
}

void CScene::Load( string sFileName )
{
	ILoader::CSceneInfos si;
	m_oLoaderManager.Load( sFileName, si );
	Load( si );
}

void CScene::Export( string sFileName )
{
	ILoader::CSceneInfos si;
	GetInfos( si );
	m_oLoaderManager.Export( sFileName, si );
}

void CScene::Clear()
{
	int nChildCount = (int)m_vChild.size();
	for( int i = 0; i < nChildCount; i++ )
	{
		IEntity* pChild = static_cast< IEntity* >( m_vChild[ 0 ] );
		pChild->Unlink();
		m_pEntityManager->DestroyEntity( pChild );
	}
	m_pRessource = NULL;
	m_pEntityManager->Clear();
}

float CScene::GetGroundHeight( float x, float z )
{
	if (m_nHeightMapID != -1)
		return m_oCollisionManager.GetMapHeight(m_nHeightMapID, x, z);// +margin;
	return -1000000.f;
}
