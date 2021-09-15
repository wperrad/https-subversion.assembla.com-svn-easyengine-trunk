#include "Scene.h"
#include "Interface.h"
#include <algorithm>
#include "IRenderer.h"
#include "FreeCamera.h"
#include "ICameraManager.h"
#include "IRessource.h"
#include "Shape.h"
#include "IEntity.h"
#include "LightEntity.h"
#include "ICollisionManager.h"
#include "Utils2/StringUtils.h"
#include "Utils2/TimeManager.h"
#include "IPathFinder.h"
#include "IFileSystem.h"
#include "EntityManager.h"
#include "MobileEntity.h"
#include "MapEntity.h"
#include "SphereEntity.h"
#include "IShader.h"
#include "IConsole.h"
#include "Bone.h"

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

CScene::CScene(EEInterface& oInterface, string ressourceFileName, string diffuseFileName) :
	CEntity(oInterface, ressourceFileName),
	m_oCameraManager(static_cast<ICameraManager&>(*oInterface.GetPlugin("CameraManager"))),
	m_oLoaderManager(static_cast<ILoaderManager&>(*oInterface.GetPlugin("LoaderManager"))),
	m_oCollisionManager(static_cast<ICollisionManager&>(*oInterface.GetPlugin("CollisionManager"))),
	m_oRessourceManager(static_cast<IRessourceManager&>(*oInterface.GetPlugin("RessourceManager"))),
	m_oPathFinder(static_cast<IPathFinder&>(*oInterface.GetPlugin("PathFinder"))),
	m_oFileSystem(static_cast<IFileSystem&>(*oInterface.GetPlugin("FileSystem"))),
	m_nHeightMapID(-1),
	m_bCollisionMapCreated(true),
	m_bHeightMapCreated(true),
	m_pCollisionGrid(NULL),
	m_sMapFirstPassShaderName("mapFirstPass"),
	m_sMapSecondPassShaderName("mapSecondPass2D"),
	m_pPlayer(NULL),
	m_bDisplayMinimap(false),
	m_fGroundMargin(-10.f),
	m_pGroundShader(NULL),
	m_pHeightMaptexture(NULL),
	m_bUseDisplacementMap(false),
	m_fDisplacementRatioHeightSize(0.04),
	m_fTiling(200.f),
	m_sDiffuseFileName(diffuseFileName),
	m_nMapLength(1000),
	m_fMapHeight(10)
{
	SetName("Scene");
	SetEntityName("Scene");
	m_pRessource = NULL;
	m_pEntityManager = static_cast<CEntityManager*>(oInterface.GetPlugin("EntityManager"));


	ICamera* pMapCamera = m_oCameraManager.CreateCamera(ICameraManager::T_MAP_CAMERA, 40.f, *m_pEntityManager);
	pMapCamera->Link(this);

	m_pMapCamera = m_oCameraManager.GetCameraFromType(ICameraManager::T_MAP_CAMERA);
	m_pMapCamera->SetWorldPosition(0, 40000, 0);
	m_pMapCamera->Pitch(-90);

	m_pMinimapTexture = CreateMinimapTexture();

	m_pPlayerMapSphere = dynamic_cast<CEntity*>(m_pEntityManager->CreateEntity("playerPointer.bme", ""));
	m_pPlayerMapSphere->SetShader(m_oRenderer.GetShader(m_sMapFirstPassShaderName));
}

CScene::~CScene()
{
}

ITexture* CScene::CreateMinimapTexture()
{
	int nMinimapWidth, nMinimapHeight;
	m_oRenderer.GetResolution(nMinimapWidth, nMinimapHeight);
	ITexture* pMinimapTexture = m_oRessourceManager.CreateRenderTexture(nMinimapWidth, nMinimapHeight, m_sMapSecondPassShaderName);
	return pMinimapTexture;
}

ITexture* CScene::GetMinimapTexture()
{
	return m_pMinimapTexture;
}

void CScene::SetRessource(string sFileName, bool bDuplicate)
{
	string ext;
	int nExtPos = (int)sFileName.find(".");
	if (nExtPos != -1) {
		ext = sFileName.substr(nExtPos + 1, 3);
		string extLower = ext;
		std::transform(ext.begin(), ext.end(), extLower.begin(), tolower);
		if (extLower == "bme")
			m_bUseDisplacementMap = false;
		else if (extLower == "bmp")
			m_bUseDisplacementMap = true;
	}
	else
	{
		exception e("CScene::SetRessource() : Erreur, vous devez indiquer une extention pour le fichier de ressource de la scene");
		throw e;
	}

	int nDotPos = (int)sFileName.find('.');
	if (m_bUseDisplacementMap) {
		m_sHMFileName = sFileName;
		int sliceCount = m_nMapLength / 500;
		string levelFileName = m_sHMFileName.substr(m_sHMFileName.find_last_of("/HMA_") + 1);
		string levelName = levelFileName.substr(0, levelFileName.find('.'));
		string levelPath = string("/levels/") + levelName + "/ground.bme";
		try {
			IAnimatableMesh* pAnimatableMesh = dynamic_cast<IAnimatableMesh*>(m_oRessourceManager.GetRessource(levelPath));
			m_pRessource = pAnimatableMesh->GetMesh(0);
		}
		catch (CFileException& e) {
			string directoryPath = "/levels/tmp";
			levelPath = directoryPath + "/ground.bme";
			try {
				IAnimatableMesh* pAnimatableMesh = dynamic_cast<IAnimatableMesh*>(m_oRessourceManager.GetRessource(levelPath));
				m_pRessource = pAnimatableMesh->GetMesh(0);
			}
			catch (CFileException& e) {
				string directoryFullPath = string("../Data") + directoryPath;
				string levelFullPath = string("../Data") + levelPath;
				CreateDirectoryA(directoryFullPath.c_str(), nullptr);
				m_pRessource = m_oRessourceManager.CreatePlane2(sliceCount, m_nMapLength, m_fMapHeight, m_sHMFileName, m_sDiffuseFileName);
				if (!CopyFileA("../Data/tmp/Ground.bme", levelFullPath.c_str(), false)) {
					CFileException e("Impossible de copier le fichier");
					e.m_sFileName = "../Data/tmp/Ground.bme";
					throw e;
				}
			}
		}
	}
	else {
		m_sHMFileName = string("hm_") + sFileName.substr(0, nDotPos) + ".bmp";
		CEntity::SetRessource(sFileName, bDuplicate);
	}
	IMesh* pMesh = static_cast< IMesh* >(m_pRessource);

	try
	{
		if (m_bUseDisplacementMap) {
			int w, h;
			pMesh->GetTexture(0)->GetDimension(w, h);
			m_fTiling = 4.f * pMesh->GetBBox()->GetDimension().m_x / w;
		}
		IBox* pBox = pMesh->GetBBox();
		m_nHeightMapID = m_oCollisionManager.LoadHeightMap(m_sHMFileName, pBox);
		m_oCollisionManager.SetGroundBoxHeight(m_nHeightMapID, m_fMapHeight);
	}
	catch( CFileNotFoundException& )
	{
		m_bHeightMapCreated = false;
	}
	

	if (m_bUseDisplacementMap) {
		nDotPos = (int)m_sDiffuseFileName.find('.');
		m_sCollisionFileName = string("collision_") + m_sDiffuseFileName.substr(0, nDotPos) + ".bmp";
	}
	else {
		nDotPos = (int)sFileName.find('.');
		m_sCollisionFileName = string("collision_") + sFileName.substr(0, nDotPos) + ".bmp";
	}
	try{
		m_oCollisionManager.LoadCollisionMap(m_sCollisionFileName, this);
		m_bCollisionMapCreated = true;
		CreateCollisionGrid();
	}
	catch (CFileNotFoundException& e) {
		m_bCollisionMapCreated = false;
	}

	m_pGroundShader = m_oRenderer.GetShader("ground");
	m_pRessource->SetShader(m_pGroundShader);
	
	m_pHeightMaptexture = static_cast< ITexture* > (m_oRessourceManager.CreateTexture2D(m_sHMFileName, false));
	m_pHeightMaptexture->SetUnitTexture(4);
	m_pHeightMaptexture->SetUnitName("heightMap");

	for (int i = 0; i < m_vEntityCallback.size(); i++) {
		m_vEntityCallback[i](nullptr, IEventDispatcher::TEntityEvent::T_LOAD_RESSOURCE, this);
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
		CEException e("Erreur : La scène ne possède pas de map");
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
	m_nHeightMapID = m_oCollisionManager.LoadHeightMap(m_sHMFileName, pMesh->GetBBox());
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

IEntity* CScene::Merge( string sRessourceName, float x, float y, float z )
{
	IEntity* pEntity = m_pEntityManager->CreateEntity( sRessourceName);
	pEntity->Link( this );
	pEntity->SetLocalPosition( x, y, z );
	return pEntity;
}

IEntity* CScene::Merge( string sRessourceName, CMatrix& oXForm )
{
	IEntity* pEntity = m_pEntityManager->CreateEntity(sRessourceName);
	pEntity->SetLocalMatrix( oXForm );
	pEntity->Link( this );
	return pEntity;
}

void CScene::Update()
{
	static int counter = 0;
	
	/*if (m_bHeightMapCreated && !m_bCollisionMapCreated && counter++ == 10) {
		CreateCollisionMap();
		CreateCollisionGrid();
	}*/
	if(!m_bCollisionMapCreated && counter++ == 20)
		CreateCollisionMap();
	
	if (!m_bHeightMapCreated && counter++ == 20 ){
		CreateHeightMap();
	}
	if (m_vMapEntities.size() == 0) {
		OnChangeSector();
	}
	CTimeManager::Instance()->Update();
	RenderScene();

	if (m_bDisplayMinimap) {
		RenderMinimap();
		m_oRenderer.SetCurrentFBO(0);
	}
	
}

void CScene::DisplayMinimap(bool display)
{
	m_bDisplayMinimap = display;
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
	else if(m_pRessource && !m_bUseDisplacementMap)
		m_pRessource->GetFileName(sFileName);
}

void CScene::SetOriginalSceneFileName(string sFileName)
{
	m_sOriginalSceneFileName = sFileName;
}


void CScene::SetDiffuseFileName(string diffuseFileName)
{
	m_sDiffuseFileName = diffuseFileName;
}

int CScene::GetCurrentHeightMapIndex()
{
	return m_nHeightMapID;
}

void CScene::SetLength(int length)
{
	m_nMapLength = length;
}

void CScene::SetHeight(float height)
{
	m_fMapHeight = height;
}

void CScene::SetHMFile(string sHMFile)
{
	m_sHMFileName = sHMFile;
}

void CScene::DeleteTempDirectories()
{
	string root;
	m_oFileSystem.GetLastDirectory(root);
	m_oFileSystem.DeleteDirectory(root + "/Levels/Tmp");
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
	if (m_pRessource) {
		if (m_pHeightMaptexture) {
			m_pHeightMaptexture->SetShader(m_pGroundShader);
			m_pGroundShader->SendUniformValues("groundHeight", m_fMapHeight);
			m_pGroundShader->SendUniformValues("tiling", m_fTiling);
			m_pHeightMaptexture->Update();
		}
		m_pRessource->Update();
	}
}

void CScene::RenderMinimap()
{
	// first pass
	m_oRenderer.SetCurrentFBO(m_pMinimapTexture->GetFrameBufferObjectId());
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
		CEntity* pEntity = dynamic_cast<CEntity*>(entities[i]);
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

ILoader::CObjectInfos* CScene::GetEntityInfos( CEntity* pEntity )
{
	ILoader::CObjectInfos* pObjectInfos = nullptr;
	pEntity->GetEntityInfos(pObjectInfos);
	return pObjectInfos;
}

void CScene::GetInfos( ILoader::CSceneInfos& si )
{
	si.m_bUseDisplacementMap = m_bUseDisplacementMap;	
	if (m_bUseDisplacementMap) {
		si.m_sDiffuseFileName = m_sDiffuseFileName;
		si.m_sSceneFileName = m_sHMFileName;
		si.m_nMapLength = m_nMapLength;
		si.m_fMapHeight = m_fMapHeight;
	}
	else
		m_pRessource->GetFileName(si.m_sSceneFileName);
	si.m_sOriginalSceneFileName = m_sOriginalSceneFileName;
	GetName( si.m_sName );
	m_oRenderer.GetBackgroundColor(si.m_oBackgroundColor);
	for( unsigned int i= 0; i < m_vChild.size(); i++ )
	{
		CEntity* pEntity = dynamic_cast< CEntity* >( m_vChild[ i ] );
		if (pEntity) {
			ILoader::CObjectInfos* pInfos = GetEntityInfos(pEntity);
			if (pInfos)
				si.m_vObject.push_back(pInfos);
		}
	}
}

void CScene::LoadSceneObject( const ILoader::CObjectInfos* pSceneObjInfos, IEntity* pParent )
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
			IEntity* pEntity = m_pEntityManager->CreateEntityFromType( sRessourceFileName, pEntityInfos->m_sTypeName, pEntityInfos->m_sObjectName);
			pEntity->SetLocalMatrix( oXForm );

			if( pEntityInfos->m_nParentBoneID != -1 )
			{
				if( pParent->GetSkeletonRoot() )
				{
					if (pEntityInfos->m_nParentBoneID == 0 && pEntity->GetSkeletonRoot() && (pEntityInfos->m_nGrandParentDummyRootID != -1) ) {
						string sDummyName;
						pEntity->GetSkeletonRoot()->GetName(sDummyName);
						string sDummyRoot = "Dummy";
						string sBodyDummyRoot = "BodyDummy";
						if (sDummyName.substr(0, sDummyRoot.size()) == "Dummy") {
							IBone* pBodyDummy = dynamic_cast< IBone* >(pParent->GetSkeletonRoot()->GetChildBoneByID(pEntityInfos->m_nGrandParentDummyRootID));
							pEntity->LinkDummyParentToDummyEntity(pParent, pBodyDummy->GetName());
						}
					}
					else {
						IBone* pBone = dynamic_cast<IBone*>(pParent->GetSkeletonRoot()->GetChildBoneByID(pEntityInfos->m_nParentBoneID));
						if (pBone)
							pParent->LinkEntityToBone(pEntity, pBone);
						else
						{
							ostringstream oss;
							oss << "Erreur dans CScene::LoadSceneObject(), fichier \"" << __FILE__ << "\" ligne " << __LINE__;
							CEException e(oss.str());
							throw e;
						}
					}
				}
			}
			else
				pEntity->Link( pParent );
			const ILoader::CAnimatedEntityInfos* pAnimatedEntityInfos = dynamic_cast< const ILoader::CAnimatedEntityInfos* >(pSceneObjInfos);
			if(pAnimatedEntityInfos && pEntity->GetSkeletonRoot() )	{
				pEntity->AddAnimation(pAnimatedEntityInfos->m_sAnimationFileName );
				pEntity->SetCurrentAnimation(pAnimatedEntityInfos->m_sAnimationFileName );
				for (map<string, float>::const_iterator it = pAnimatedEntityInfos->m_mAnimationSpeed.begin(); it != pAnimatedEntityInfos->m_mAnimationSpeed.end(); it++) {
					pEntity->SetAnimationSpeed(CMobileEntity::s_mStringToAnimation[it->first], it->second);
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
	Clear();
	m_oRenderer.SetBackgroundColor(si.m_oBackgroundColor);
	if (si.m_bUseDisplacementMap) {
		m_nMapLength = si.m_nMapLength;
		m_fMapHeight = si.m_fMapHeight;
		m_sDiffuseFileName = si.m_sDiffuseFileName;
	}
	SetRessource(si.m_sSceneFileName);
	m_sOriginalSceneFileName = si.m_sOriginalSceneFileName;
	for( unsigned int i = 0; i < si.m_vObject.size(); i++ )
	{
		const ILoader::CObjectInfos* pSceneObjInfos = si.m_vObject.at( i );
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
	m_oCameraManager.UnlinkCameras();
	int nChildCount = (int)m_vChild.size();
	for( int i = 0; i < nChildCount; i++ )
	{
		IEntity* pChild = dynamic_cast< IEntity* >( m_vChild[ 0 ] );
		pChild->Unlink();
		m_pEntityManager->DestroyEntity( pChild );
	}
	m_pRessource = NULL;
	m_pEntityManager->Clear();
	m_oCollisionManager.ClearHeightMaps();
}

float CScene::GetGroundHeight( float x, float z )
{
	if (m_nHeightMapID != -1)
		return m_oCollisionManager.GetMapHeight(m_nHeightMapID, x, z);// +margin;
	return -1000000.f;
}
