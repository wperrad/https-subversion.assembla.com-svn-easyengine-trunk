#include "CollisionManager.h"
#include "IRessource.h"
#include "IShader.h"
#include "ILoader.h"
#include "HeightMap.h"
#include "IGeometry.h"
#include "Utils2/RenderUtils.h"
#include "IEntity.h"
#include "Interface.h"
#include "IFileSystem.h"
#include <set>

IMesh* CCollisionManager::s_pMesh = NULL;

CCollisionManager* g_pCurrentCollisionManager = NULL;

CCollisionManager::CCollisionManager(EEInterface& oInterface) :
m_oInterface(oInterface),
m_oRenderer(static_cast<IRenderer&>(*m_oInterface.GetPlugin("Renderer"))),
m_oLoaderManager(static_cast<ILoaderManager&>(*m_oInterface.GetPlugin("LoaderManager"))),
m_pFileSystem(static_cast<IFileSystem*>(m_oInterface.GetPlugin("FileSystem"))),
m_oGeometryManager(static_cast<IGeometryManager&>(*m_oInterface.GetPlugin("GeometryManager"))),
m_nHeightMapPrecision( 1 ),
m_pGround(NULL),
m_sCustomName("a"),
m_fCustomValue(0.f),
m_fGroundWidth(0),
m_fGroundHeight(0),
m_fGridCellSize(1000.f),
m_fGridHeight(800.f),
m_pCollisionGrid(NULL),
m_pScene(NULL),
m_fScreenRatio(m_fScreenRatio),
m_pEntityManager(NULL),
m_bEnableHMHack(false),
m_bEnableHMHack2(false)
{
	g_pCurrentCollisionManager = this;
	m_oRenderer.GetResolution(m_nScreenWidth, m_nScreenHeight);
	m_fScreenRatio = (float)m_nScreenWidth / (float)m_nScreenHeight;
	
}

void CCollisionManager::DisplayHeightMap(IMesh* pMesh)
{
	m_oRenderer.GetBackgroundColor(m_oOriginBackgroundColor);
	ILoader::CTextureInfos ti;
	CreateHeightMap(pMesh, ti);
	s_pMesh = pMesh;
	m_oRenderer.AbonneToRenderEvent(OnRenderHeightMap);
}

void CCollisionManager::StopDisplayHeightMap()
{
	m_oRenderer.DesabonneToRenderEvent(OnRenderHeightMap);
	m_oRenderer.SetBackgroundColor(m_oOriginBackgroundColor.m_x, m_oOriginBackgroundColor.m_y, m_oOriginBackgroundColor.m_z);
}

void CCollisionManager::DisplayCollisionMap()
{
	m_oRenderer.AbonneToRenderEvent(OnRenderCollisionMapCallback);
}

void CCollisionManager::StopDisplayCollisionMap()
{
	m_oRenderer.DesabonneToRenderEvent(OnRenderCollisionMapCallback);
}

void CCollisionManager::OnRenderCollisionMapCallback(IRenderer*)
{
	g_pCurrentCollisionManager->OnRenderCollisionMap();
}

void CCollisionManager::RenderCollisionGeometry(IShader* pCollisionShader, const CMatrix& groundModel, const IBox* const pBox)
{
	pCollisionShader->SendUniformValues("h", pBox->GetDimension().m_y);
	pCollisionShader->SendUniformValues("zMin", pBox->GetMinPoint().m_y);
	pCollisionShader->SendUniformValues("scale", m_fWorldToScreenScaleFactor);
	pCollisionShader->SendUniformMatrix4("modelMatrix", groundModel, true);
	pCollisionShader->SendUniformValues("isGround", 1.f);
	pCollisionShader->SendUniformValues("isGrid", 0.f);
	pCollisionShader->SendUniformValues(m_sCustomName, m_fCustomValue);

	m_pGround->Update();

	pCollisionShader->SendUniformValues("isGround", 0.f);
	for (vector<IEntity*>::iterator it = m_vCollideObjects.begin(); it != m_vCollideObjects.end(); it++) {
		const CMatrix& model = (*it)->GetWorldMatrix();
		pCollisionShader->SendUniformMatrix4("modelMatrix", model, true);
		(*it)->Update();
	}

	pCollisionShader->SendUniformValues("isGround", 0.f);
	pCollisionShader->SendUniformValues("isGrid", 1.f);
	for (vector<IEntity*>::iterator it = m_vGridElements.begin(); it != m_vGridElements.end(); it++) {
		const CMatrix& model = (*it)->GetWorldMatrix();
		pCollisionShader->SendUniformMatrix4("modelMatrix", model, true);
		(*it)->Update();
	}
	
}

void CCollisionManager::OnRenderCollisionMap()
{
	IShader* pOrgShader = m_pGround->GetShader();
	IShader* pCollisionShader = m_oRenderer.GetShader("collision");
	pCollisionShader->Enable(true);
	m_pGround->SetShader(pCollisionShader);

	CMatrix model, oProj;
	int nWidth, nHeight;
	m_oRenderer.GetResolution(nWidth, nHeight);
	oProj.m_00 = (float)nHeight / (float)nWidth;

	vector<IShader*> vBackStaticObjectShader;
	vector<IShader*> vBackLineShader;

	GetOriginalShaders(m_vCollideObjects, vBackStaticObjectShader);
	SetCollisionShaders(m_vCollideObjects, pCollisionShader);

	GetOriginalShaders(m_vGridElements, vBackLineShader);
	SetCollisionShaders(m_vGridElements, pCollisionShader);

	CMatrix oBakProj;
	m_oRenderer.GetProjectionMatrix(oBakProj);
	m_oRenderer.SetProjectionMatrix(oProj);

	bool wasCullingEnabled = m_oRenderer.IsCullingEnabled();
	if (wasCullingEnabled)
		m_oRenderer.CullFace(false);
	m_oRenderer.EnableDepthTest(false);

	RenderCollisionGeometry(pCollisionShader, model, m_pGround->GetBBox());

	m_oRenderer.EnableDepthTest(true);

	if (wasCullingEnabled)
		m_oRenderer.CullFace(true);

	pCollisionShader->Enable(false);
	m_pGround->SetShader(pOrgShader);

	RestoreOriginalShaders(vBackStaticObjectShader, m_vCollideObjects);
	RestoreOriginalShaders(vBackLineShader, m_vGridElements);

	m_oRenderer.SetProjectionMatrix(oBakProj);
}

void CCollisionManager::ComputeGroundMapDimensions(IMesh* pMesh, float& width, float& height, float& groundToScreenScaleFactor)
{
	IBox* pBox = pMesh->GetBBox();
	if (pBox->GetDimension().m_x <= pBox->GetDimension().m_z * m_fScreenRatio)
	{
		width = (pBox->GetDimension().m_x * (float)m_nScreenHeight) / pBox->GetDimension().m_z;
		height = (float)m_nScreenHeight;
		groundToScreenScaleFactor = pBox->GetDimension().m_z / 2.f;
	}
	else
	{
		width = (float)m_nScreenWidth;
		height = pBox->GetDimension().m_z * (float)m_nScreenWidth / pBox->GetDimension().m_x;
		groundToScreenScaleFactor = pBox->GetDimension().m_x / 2.f;
	}
	width = (float)(((int)width / 4) * 4);
	height = ((int)height / 4) * 4;
}

void CCollisionManager::CreateCollisionMap(ILoader::CTextureInfos& ti, vector<IEntity*> collides, IEntity* pScene, IRenderer::TPixelFormat format)
{
	m_pScene = pScene;

	m_pGround = dynamic_cast<IMesh*>(pScene->GetRessource());
	if (!m_pGround) {
		CEException e("Erreur, aucun mesh défini pour la scene");
		throw e;
	}
	
	IShader* pOrgShader = m_pGround->GetShader();
	IShader* pCollisionShader = m_oRenderer.GetShader("collision");
	pCollisionShader->Enable(true);
	m_pGround->SetShader(pCollisionShader);

	CMatrix model, oProj;
	oProj.m_00 = 1.f / m_fScreenRatio;
	const IBox* pBox = m_pGround->GetBBox();

	ComputeGroundMapDimensions(m_pGround, m_fGroundMapWidth, m_fGroundMapHeight, m_fWorldToScreenScaleFactor);
	int subdivisionCount = 30;

	int cellMapSize = m_fGroundMapWidth / subdivisionCount;

	float fOriginMapX = ((float)m_nScreenWidth - m_fGroundMapWidth) / 2.f;
	float fOriginMapY = ((float)m_nScreenHeight - m_fGroundMapHeight) / 2.f;

	m_fGroundWidth = pBox->GetDimension().m_x;
	m_fGroundHeight = pBox->GetDimension().m_z;

	m_vCollideObjects = collides;

	vector<IShader*> vBackStaticObjectShader;
	vector<IShader*> vBackLineShader;

	GetOriginalShaders(m_vCollideObjects, vBackStaticObjectShader);
	SetCollisionShaders(m_vCollideObjects, pCollisionShader);

	GetOriginalShaders(m_vGridElements, vBackLineShader);
	SetCollisionShaders(m_vGridElements, pCollisionShader);

	CMatrix oBakProj;
	m_oRenderer.GetProjectionMatrix(oBakProj);
	m_oRenderer.SetProjectionMatrix(oProj);

	bool wasCullingEnabled = m_oRenderer.IsCullingEnabled();
	if (wasCullingEnabled)
		m_oRenderer.CullFace(false);
	m_oRenderer.EnableDepthTest(false);

	m_oRenderer.BeginRender();

	RenderCollisionGeometry(pCollisionShader, model, pBox);

	m_oRenderer.ReadPixels(fOriginMapX, fOriginMapY, m_fGroundMapWidth, m_fGroundMapHeight, ti.m_vTexels, format);
	ti.m_nWidth = (int)m_fGroundMapWidth;
	ti.m_nHeight = (int)m_fGroundMapHeight;

	m_oRenderer.EndRender();

	m_oRenderer.EnableDepthTest(true);

	if (wasCullingEnabled)
		m_oRenderer.CullFace(true);

	pCollisionShader->Enable(false);
	m_pGround->SetShader(pOrgShader);

	RestoreOriginalShaders(vBackStaticObjectShader, m_vCollideObjects);
	RestoreOriginalShaders(vBackLineShader, m_vGridElements);

	m_oRenderer.SetProjectionMatrix(oBakProj);
}

void CCollisionManager::LoadCollisionMap(string sFileName, IEntity* pScene)
{
	m_oLoaderManager.Load(sFileName, m_oCollisionMap);
	IMesh* pGround = dynamic_cast<IMesh*>(pScene->GetRessource());
	m_pGround = pGround;
	m_pScene = pScene;
	if (m_pGround) {
		IBox* pBox = m_pGround->GetBBox();
		m_fGroundWidth = pBox->GetDimension().m_x;
		m_fGroundHeight = pBox->GetDimension().m_z;
		ComputeGroundMapDimensions(m_pGround, m_fGroundMapWidth, m_fGroundMapHeight, m_fWorldToScreenScaleFactor);
	}
}

void CCollisionManager::SetHeightMapPrecision( int nPrecision )
{
	m_nHeightMapPrecision = nPrecision;
}

void CCollisionManager::CreateHeightMap( IMesh* pGround, ILoader::CTextureInfos& ti, IRenderer::TPixelFormat format )
{
	if (!m_pGround)
		m_pGround = pGround;
 	IShader* pOrgShader = pGround->GetShader();
	IShader* pHMShader = NULL;
	if(!m_bEnableHMHack && !m_bEnableHMHack2)
		pHMShader = m_oRenderer.GetShader("hm");
	else
		pHMShader = m_oRenderer.GetShader("hmHack");
	pHMShader->Enable(true);
	pGround->SetShader(pHMShader);

	CMatrix oModelView, oProj;
	int nWidth, nHeight;
	m_oRenderer.GetResolution(nWidth, nHeight);
	float fScreenRatio = (float)nWidth / (float)nHeight;
	oProj.m_00 = 1.f / fScreenRatio;
	const IBox* pBox = pGround->GetBBox();
	float maxLenght = pBox->GetDimension().m_x;
	if (maxLenght < pBox->GetDimension().m_z)
		maxLenght = pBox->GetDimension().m_z;
	
	ComputeGroundMapDimensions(pGround, m_fGroundMapWidth, m_fGroundMapHeight, m_fWorldToScreenScaleFactor);
	if(m_bEnableHMHack)
		m_fWorldToScreenScaleFactor *= 1.70068f;
	else if (m_bEnableHMHack2) {
		m_fWorldToScreenScaleFactor *= 2.55103f;
	}
	float fOriginMapX = ((float)nWidth - m_fGroundMapWidth) / 2.f;
	float fOriginMapY = ((float)nHeight - m_fGroundMapHeight) / 2.f;

	pHMShader->SendUniformValues("h", pBox->GetDimension().m_y);
	float zmin = pBox->GetMinPoint().m_y;
	if (m_bEnableHMHack) {
		zmin += 210.f; // temporary hack
	}
	else if (m_bEnableHMHack2) {
		static float value = 210.f;
		zmin += value; // temporary hack
	}
	pHMShader->SendUniformValues("zMin", zmin);
	pHMShader->SendUniformValues("scale", m_fWorldToScreenScaleFactor);

	CMatrix oBakProj;
	m_oRenderer.GetProjectionMatrix(oBakProj);
	m_oRenderer.SetProjectionMatrix(oProj);

	m_oRenderer.CullFace(false);
	m_oRenderer.BeginRender();
	pGround->Update();

	m_oRenderer.ReadPixels(fOriginMapX, fOriginMapY, m_fGroundMapWidth, m_fGroundMapHeight, ti.m_vTexels, format);
	ti.m_nWidth = (int)m_fGroundMapWidth;
	ti.m_nHeight = (int)m_fGroundMapHeight;
	m_oRenderer.EndRender();
	m_oRenderer.CullFace(true);

	pHMShader->Enable(false);
	pGround->SetShader(pOrgShader);
	m_oRenderer.SetProjectionMatrix(oBakProj);
}

void CCollisionManager::CreateHeightMap(string sFileName)
{
	if (sFileName.find(".bme") == -1)
		sFileName += ".bme";
	IEntity* pEntity = NULL;
	try
	{
		pEntity = m_pEntityManager->CreateEntity(sFileName, "");
	}
	catch (CEException& e)
	{
		string sError;
		e.GetErrorMessage(sError);
		string s = string("Erreur : ") + sError;
		throw e;
	}
	if (pEntity)
	{
		IMesh* pMesh = dynamic_cast< IMesh* >(pEntity->GetRessource());
		if (pMesh)
		{
			IMesh* pGround = static_cast<IMesh*>(m_pScene->GetRessource());
			string sSceneFileName;
			pGround->GetFileName(sSceneFileName);
			if (sSceneFileName == sFileName)
				EnableHMHack(true);
			else
				EnableHMHack2(true);
			ILoader::CTextureInfos ti;
			CreateHeightMap(pMesh, ti, IRenderer::T_BGR);
			ti.m_ePixelFormat = ILoader::eBGR;
			string sTextureFileName = sFileName.substr(0, sFileName.find('.'));
			sTextureFileName = string("HM_") + sTextureFileName + ".bmp";
			m_oLoaderManager.Export(sTextureFileName, ti);
		}
	}
}

void CCollisionManager::CreateHeightMapWithoutRender(string sFileName)
{
	ILoader::CTextureInfos ti;
	ILoader::CAnimatableMeshData ami;
	m_oLoaderManager.Load(sFileName, ami);
	ILoader::CMeshInfos& mi = ami.m_vMeshes[0];
	CVector dim = mi.m_pBoundingBox->GetDimension();
	int nCellCount = dim.m_x * dim.m_z;
	int quadCount = mi.m_vIndex.size() / 6;
	set<float> setx;
	set<float> sety;
	set<float> setz;
	map<pair<float, float>, float> mxz;
	float ymin = mi.m_vVertex[1];
	float ymax = ymin;
	for (int i = 0; i < mi.m_vVertex.size() / 3; i++) {
		float x = mi.m_vVertex[3 * i];
		float y = mi.m_vVertex[3 * i + 1];
		float z = mi.m_vVertex[3 * i + 2];
		setx.insert(x);
		sety.insert(y);
		setz.insert(z);
		mxz[pair<float, float>(x, z)] = y;
		if (ymin > y)
			ymin = y;
		if (ymax < y)
			ymax = y;
	}
	float d = dim.m_x / setx.size();
	int width = 64; // setx.size() + 1;
	int height = 64; // setz.size() + 1;
	ti.m_vTexels.resize(3 * width * height);
	memset(&ti.m_vTexels[0], 0, ti.m_vTexels.size());

	
	for (map<pair<float, float>, float>::iterator it = mxz.begin(); it != mxz.end(); it++) {
		float x = it->first.first;
		float y = it->second;
		float z = it->first.second;
		x += dim.m_x / 2.f;
		z += dim.m_z / 2.f;
		int pixelx = x / d;
		int pixelz = z / d;
		int pixelIndex = pixelx + pixelz * setx.size();
		if (pixelIndex < ti.m_vTexels.size()/3) {
			ti.m_vTexels[3 * pixelIndex] = 255 * (0.5f + y / dim.m_y);
			ti.m_vTexels[3 * pixelIndex + 1] = 255 * (0.5f + y / dim.m_y);
			ti.m_vTexels[3 * pixelIndex + 2] = 255 * (0.5f + y / dim.m_y);
		}
	}
	ti.m_nWidth = width;
	ti.m_nHeight = height;
	ti.m_ePixelFormat = ILoader::eBGR;
	string groundName = sFileName.substr(0, sFileName.find('.'));
	string textureFileName = "HM_" + groundName + "_test.bmp";
	m_oLoaderManager.Export(textureFileName, ti);
}

void CCollisionManager::GetOriginalShaders(const vector<IEntity*>& staticObjects, vector<IShader*>& vBackupStaticObjectShader)
{
	for (vector<IEntity*>::const_iterator it = staticObjects.begin(); it != staticObjects.end(); it++) {
		IMesh* pMesh = dynamic_cast<IMesh*>((*it)->GetRessource());
		if (pMesh)
			vBackupStaticObjectShader.push_back(pMesh->GetShader());
	}
}

void CCollisionManager::SetCollisionShaders(const vector<IEntity*>& staticObjects, IShader* pCollisionShader)
{
	for (vector<IEntity*>::const_iterator it = staticObjects.begin(); it != staticObjects.end(); it++) {
		(*it)->SetShader(pCollisionShader);
	}
}

void CCollisionManager::RestoreOriginalShaders(const vector<IShader*>& vBackupStaticObjectShader, vector<IEntity*>& staticObjects)
{
	int i = 0;
	for (vector<IEntity*>::iterator it = staticObjects.begin(); it != staticObjects.end(); it++) {
		(*it)->SetShader(vBackupStaticObjectShader[i++]);
	}
}

void CCollisionManager::OnRenderHeightMap( IRenderer* pRenderer )
{
	pRenderer->SetBackgroundColor( 0, 0, 1 );

	IShader* pOrgShader = s_pMesh->GetShader();
	IShader* pHMShader = pRenderer->GetShader( "hm" );
	pHMShader->Enable( true );
	s_pMesh->SetShader( pHMShader );
	
	CMatrix oModelView, oProj;
	int nWidth, nHeight;
	pRenderer->GetResolution( nWidth, nHeight );
	float fScreenRatio = (float)nWidth / (float)nHeight;
	oProj.m_00 = 1.f / fScreenRatio;
	const IBox* pBox = s_pMesh->GetBBox();
	float maxLenght = pBox->GetDimension().m_x ;
	if( maxLenght < pBox->GetDimension().m_y )
		maxLenght = pBox->GetDimension().m_y;
	float scale = ( maxLenght / 2.f );
	pHMShader->SendUniformValues( "h", pBox->GetDimension().m_z );
	pHMShader->SendUniformValues( "zMin", pBox->GetMinPoint().m_z );
	pHMShader->SendUniformValues( "zMax", pBox->GetMinPoint().m_z + pBox->GetDimension().m_z );
	pHMShader->SendUniformValues( "scale", scale );

	pRenderer->SetModelViewMatrix( oModelView );
	CMatrix oBakProj;
	pRenderer->GetProjectionMatrix( oBakProj );
	pRenderer->SetProjectionMatrix( oProj );

	s_pMesh->Update();

	pHMShader->Enable( false );
	s_pMesh->SetShader( pOrgShader );
	pRenderer->SetProjectionMatrix( oBakProj );
}

int CCollisionManager::LoadHeightMap( string sFileName, IBox* pBox)
{
	int nID = 0;
	map<string, int>::iterator it = m_mMapFileToId.find(sFileName);
	if (it == m_mMapFileToId.end()) {
		CHeightMap hm(m_oInterface, sFileName, *pBox);
		hm.SetPrecision(m_nHeightMapPrecision);
		nID = (int)m_mHeigtMap.size();
		m_mHeigtMap[nID] = hm;
		m_mMapFileToId[sFileName] = nID;
	}
	else
		nID = it->second;
	return nID;
}

void CCollisionManager::LoadHeightMap(string sFileName, vector< vector< unsigned char > >& vPixels)
{
	ILoader::CTextureInfos ti;
	m_oLoaderManager.Load(sFileName, ti);
	vPixels.clear();
	vPixels.resize(ti.m_nWidth);
	for (int i = 0; i < ti.m_nWidth; i++)
		for (int j = 0; j < ti.m_nHeight; j++)
			vPixels[i].push_back(ti.m_vTexels[j * (ti.m_nWidth + 1)]);
}

float CCollisionManager::GetMapHeight( int nHeightMapID, float xModel, float zModel )
{
	map< int, CHeightMap >::iterator itMap = m_mHeigtMap.find( nHeightMapID );
	float fInterpolate = itMap->second.GetHeight( xModel, zModel );
	return fInterpolate;
}

void CCollisionManager::ExtractHeightMapFromTexture( string sFileName, string sOutFileName)
{
	CHeightMap::ExtractHeightMapFromTexture( sFileName, m_oLoaderManager, m_pFileSystem, sOutFileName );
}

bool CCollisionManager::IsSegmentInsideSegment( float fS1Center, float fS1Radius, float fS2Center, float fS2Radius )
{
	return ( ( fS1Center + fS1Radius ) > ( fS2Center - fS2Radius ) ) && ( ( fS1Center - fS1Radius ) < ( fS2Center + fS2Radius ) );
}

bool CCollisionManager::IsIntersection( const IBox& b, const ISphere& s )
{
	CMatrix invBoxWorldMatrix, oWorldMatrix;
	b.GetTM( oWorldMatrix );
	oWorldMatrix.GetInverse( invBoxWorldMatrix );
	CVector vBoxBaseSphereCenter = invBoxWorldMatrix * s.GetCenter();
	bool bInsideX = IsSegmentInsideSegment( 0, b.GetDimension().m_x / 2.f, vBoxBaseSphereCenter.m_x, s.GetRadius() ); //vBoxBaseSphereCenter.m_x - s.GetRadius() < b.GetDimension().m_x / 2.f && vBoxBaseSphereCenter.m_x + s.GetRadius() > - b.GetDimension().m_x / 2.f;
	bool bInsideY = IsSegmentInsideSegment( 0, b.GetDimension().m_y / 2.f, vBoxBaseSphereCenter.m_y, s.GetRadius() );
	bool bInsideZ = IsSegmentInsideSegment( 0, b.GetDimension().m_z / 2.f, vBoxBaseSphereCenter.m_z, s.GetRadius() );
	return ( bInsideX && bInsideY && bInsideZ );
}

float GetMinx( const vector< CVector >& vPoints )
{
	float fMin = vPoints[ 0 ].m_x;
	for( int i = 1; i < vPoints.size(); i++ )
	{
		if( fMin > vPoints[ i ].m_x )
			fMin = vPoints[ i ].m_x;
	}
	return fMin;
}

float GetMiny( const vector< CVector >& vPoints )
{
	float fMin = vPoints[ 0 ].m_y;
	for( int i = 1; i < vPoints.size(); i++ )
	{
		if( fMin > vPoints[ i ].m_y )
			fMin = vPoints[ i ].m_y;
	}
	return fMin;
}

float GetMinz( const vector< CVector >& vPoints )
{
	float fMin = vPoints[ 0 ].m_z;
	for( int i = 1; i < vPoints.size(); i++ )
	{
		if( fMin > vPoints[ i ].m_z )
			fMin = vPoints[ i ].m_z;
	}
	return fMin;
}

float GetMaxx( const vector< CVector >& vPoints )
{
	float fMax = vPoints[ 0 ].m_x;
	for( int i = 1; i < vPoints.size(); i++ )
	{
		if( fMax < vPoints[ i ].m_x )
			fMax = vPoints[ i ].m_x;
	}
	return fMax;
}

float GetMaxy( const vector< CVector >& vPoints )
{
	float fMax = vPoints[ 0 ].m_y;
	for( int i = 1; i < vPoints.size(); i++ )
	{
		if( fMax < vPoints[ i ].m_y )
			fMax = vPoints[ i ].m_y;
	}
	return fMax;
}

float GetMaxz( const vector< CVector >& vPoints )
{
	float fMax = vPoints[ 0 ].m_z;
	for( int i = 1; i < vPoints.size(); i++ )
	{
		if( fMax < vPoints[ i ].m_z )
			fMax = vPoints[ i ].m_z;
	}
	return fMax;
}

bool CCollisionManager::IsIntersection( const ISegment& s, const IBox& b2 )
{
	return true;
}

bool CCollisionManager::IsIntersection( const ISegment& s, const CVector& oCircleCenter, float fCircleRadius )
{
	CVector H;
	s.ComputeProjectedPointOnLine( oCircleCenter, H );

	return false;
}

void CCollisionManager::Get2DLineIntersection( const CVector2D& oLine1First, const CVector2D& oLine1Last, const CVector2D& oLine2First, const CVector2D& oLine2Last, CVector2D& oIntersection )
{
	ISegment2D* pL1 = m_oGeometryManager.CreateSegment2D( oLine1First, oLine1Last );
	ISegment2D* pL2 = m_oGeometryManager.CreateSegment2D( oLine2First, oLine2Last );
	Get2DLineIntersection( *pL1, *pL2, oIntersection );
}

void CCollisionManager::Get2DLineIntersection( const ISegment2D& oSeg1, const ISegment2D& oSeg2, CVector2D& oIntersection )
{
	float a1, b1, c1, a2, b2, c2;
	oSeg1.ComputeLineEquation( a1, b1, c1 );
	oSeg2.ComputeLineEquation( a2, b2, c2 );
	oIntersection.m_x = 1 / a1 * ( b1 * ( (a2 * c1 - a1 * c2 ) / ( a2 * b1 - a1 * b2 )  ) - c1 );
	oIntersection.m_y = ( a1 * c2 - a2 * c1 ) / ( a2 * b1 - a1 * b2 );
}

bool CCollisionManager::IsSegmentRectIntersect( const ISegment2D& s, float fRectw, float fRecth, const CMatrix2X2& oRectTM )
{
	CVector2D S1, S2;
	s.GetPoints( S1, S2 );
	return IsSegmentRectIntersect( S1, S2, fRectw, fRecth, oRectTM );

}

bool CCollisionManager::IsSegmentRectIntersect( const CVector2D& S1, const CVector2D& S2, float fRectw, float fRecth, const CMatrix2X2& oRectTM )
{
	CVector2D x( 1, 0 );

	CVector2D R0 = CVector2D( -fRectw / 2.f, -fRecth / 2.f );
	CVector2D R1 = CVector2D( fRectw / 2.f, -fRecth / 2.f );
	CVector2D R2 = CVector2D( fRectw / 2.f, fRecth / 2.f );
	CVector2D R3 = CVector2D( -fRectw / 2.f, fRecth / 2.f );
	CMatrix2X2 oRectInvTM;
	oRectTM.GetInverse( oRectInvTM );
	
	CVector2D S1Inv = oRectInvTM * S1;
	CVector2D S2Inv = oRectInvTM * S2;

	float fMinx = R0.m_x, fMaxx = R1.m_x, fMinz = R0.m_y, fMaxz = R2.m_y;
	float fSegMinx = S1Inv.m_x;
	if( fSegMinx > S2Inv.m_x ) fSegMinx = S2Inv.m_x;
	if( fSegMinx > fMaxx )
		return false;
	float fSegMaxx = S1Inv.m_x;
	if( fSegMaxx < S2Inv.m_x ) fSegMaxx = S2Inv.m_x;
	if( fSegMaxx < fMinx )
		return false;
	float fSegMinz = S1Inv.m_y;
	if( fSegMinz > S2Inv.m_y ) fSegMinz = S2Inv.m_y;
	if( fSegMinz > fMaxz )
		return false;
	float fSegMaxz = S1Inv.m_y;
	if( fSegMaxz < S2Inv.m_y ) fSegMaxz = S2Inv.m_y;
	if( fSegMaxz < fMinz )
		return false;

	CVector2D R0tm = oRectTM * CVector2D( -fRectw / 2.f, -fRecth / 2.f );
	CVector2D R1tm = oRectTM * CVector2D( fRectw / 2.f, -fRecth / 2.f );
	CVector2D R2tm = oRectTM * CVector2D( fRectw / 2.f, fRecth / 2.f );
	CVector2D R3tm = oRectTM * CVector2D( -fRectw / 2.f, fRecth / 2.f );

	float alpha = acosf( ( ( S2 - S1 ) * x ) / ( S2 - S1 ).Norm() ) * 180.f / 3.1415927f;
	CMatrix2X2 oSegTM = CMatrix2X2::GetRotation( alpha ), oSegTMInv;
	oSegTM.AddTranslation( S1 );
	oSegTM.GetInverse( oSegTMInv );

	CVector2D R0Inv = oSegTMInv * R0tm;
	CVector2D R1Inv = oSegTMInv * R1tm;
	CVector2D R2Inv = oSegTMInv * R2tm;
	CVector2D R3Inv = oSegTMInv * R3tm;

	CVector2D S3Inv = oSegTMInv * S2;

	fMinx = R0Inv.m_y;
	if( R1Inv.m_x < fMinx ) fMinx = R1Inv.m_x;
	if( R2Inv.m_x < fMinx ) fMinx = R2Inv.m_x;
	if( R3Inv.m_x < fMinx ) fMinx = R3Inv.m_x;
	if( fMinx > S3Inv.m_x )
		return false;

	fMaxx = R0Inv.m_y;
	if( R1Inv.m_x > fMaxx ) fMaxx = R1Inv.m_y;
	if( R2Inv.m_x > fMaxx ) fMaxx = R2Inv.m_y;
	if( R3Inv.m_x > fMaxx ) fMaxx = R3Inv.m_y;
	if( fMaxx < 0 )
		return false;

	fMinz = R0Inv.m_y;
	if( R1Inv.m_y < fMinz ) fMinz = R1Inv.m_y;
	if( R2Inv.m_y < fMinz ) fMinz = R2Inv.m_y;
	if( R3Inv.m_y < fMinz ) fMinz = R3Inv.m_y;
	if( fMinz > 0 )
		return false;

	fMaxz = R0Inv.m_y;
	if( R1Inv.m_y > fMaxz ) fMaxz = R1Inv.m_y;
	if( R2Inv.m_y > fMaxz ) fMaxz = R2Inv.m_y ;
	if( R3Inv.m_y > fMaxz ) fMaxz = R3Inv.m_y;
	if( fMaxz < 0 )
		return false;

	return true;
}

void CCollisionManager::SetGroundBoxHeight(int nMapId, float height)
{
	m_mHeigtMap[nMapId].GetModelBBox()->SetY(height);
}

void CCollisionManager::SetGroundBoxMinPoint(int nMapId, float min)
{
	m_mHeigtMap[nMapId].GetModelBBox()->SetY(min);
}

IBox* CCollisionManager::GetGroundBox(int nMapId)
{
	return m_mHeigtMap[nMapId].GetModelBBox();
}

string CCollisionManager::GetName()
{
	return "CollisionManager";
}

IHeightMap*	CCollisionManager::GetHeightMap(int index)
{
	map< int, CHeightMap>::iterator it = m_mHeigtMap.find(index);
	if(it != m_mHeigtMap.end())
		return &m_mHeigtMap[index];
	return nullptr;
}

void CCollisionManager::ClearHeightMaps()
{
	m_mHeigtMap.clear();
	m_mMapFileToId.clear();
}

void CCollisionManager::SendCustomUniformValue(string name, float value)
{
	m_sCustomName = name;
	m_fCustomValue = value;
}

void CCollisionManager::DisplayGrid()
{
	CVector first, last;
	int rowCount = m_fGroundHeight / m_fGridCellSize + 1;
	int columnCount = m_fGroundWidth / m_fGridCellSize + 1;
	int w = m_fGroundWidth;
	int h = m_fGroundHeight;
	int s = m_fGridCellSize;
	if (w % s > 0)
		columnCount++;
	if (h % s > 0)
		rowCount++;

	float lineWidth = 15.f;
	float y = 1130.f;

	for (int i = 0; i < rowCount; i++) {
		first.m_x = -m_fGroundWidth / 2;
		first.m_z = -m_fGroundHeight / 2 + i * m_fGridCellSize;

		first.m_y = y;

		IEntity* line = m_pEntityManager->CreateCylinder(lineWidth, m_fGroundWidth);
		line->Link(m_pScene);
		line->SetLocalPosition(first);
		line->Roll(-90.f);
		line->LocalTranslate(0, m_fGroundWidth / 2, 0);
		line->Colorize(1, 0, 0, 1);
		m_vGridElements.push_back(line);
	}

	for (int i = 0; i < columnCount; i++) {
		first.m_x = -m_fGroundWidth / 2 + i * m_fGridCellSize;
		first.m_z = -m_fGroundHeight / 2;
		
		first.m_y = y;

		IEntity* line = m_pEntityManager->CreateCylinder(lineWidth, m_fGroundWidth);
		line->Link(m_pScene);
		line->SetLocalPosition(first);
		line->Pitch(90.f);
		line->LocalTranslate(0, m_fGroundWidth / 2, 0);
		m_vGridElements.push_back(line);
	}
}

void CCollisionManager::MarkBox(int row, int column, float r, float g, float b, IEntityManager* pEntityManager)
{
	float x, z;
	GetPositionFromCellCoord(row, column, x, z);
	IEntity* pSphere = pEntityManager->CreateSphere(m_fGridCellSize*3. / 8.);
	IShader* pColorShader = m_oRenderer.GetShader("color");
	pSphere->SetShader(pColorShader);
	pSphere->Colorize(r, g, b, 0.5f);
	pSphere->SetLocalPosition(x + m_fGridCellSize / 2, m_fGridHeight , z + m_fGridCellSize / 2);
	pSphere->Link(m_pScene);
	m_vGridElements.push_back(pSphere);
}

void CCollisionManager::MarkMapBox(int row, int column, int r, int g, int b)
{
	float x0, y0, x1, y1;
	GetPositionFromCellCoord(row, column, x0, y0);
	GetPositionFromCellCoord(row + 1, column + 1, x1, y1);

	float mapX0 = WorldToPixel(x0);
	float mapX1 = WorldToPixel(x1);

	int subdivisionCount = 30;
	float fmapBoxSize = mapX1 - mapX0;
	int mapBoxSize = fmapBoxSize;
	int rowCount = m_oCollisionMap.m_nHeight / mapBoxSize;
	int columnCount = m_oCollisionMap.m_nWidth / mapBoxSize;

	bool isObstacle = false;

	float fx = (float)column * fmapBoxSize;
	int ix = (int)fx;
	float fy = (float)row * fmapBoxSize;
	int iy = (int)fy;

	for (int i = 0; i < mapBoxSize; i++) {

		int index = 3 * (ix + i * m_oCollisionMap.m_nWidth + iy * m_oCollisionMap.m_nWidth);
		int fillSize = 3 * mapBoxSize;
		if (column == columnCount) {
			fillSize = (m_oCollisionMap.m_nWidth * 3) - column * 3 * mapBoxSize;
		}
		if (index < m_oCollisionMap.m_vTexels.size()) {
			for (int j = 0; j < fillSize / 3; j++) {
				m_oCollisionMap.m_vTexels[index + 3 * j] = b;
				m_oCollisionMap.m_vTexels[index + 3 * j + 1] = g;
				m_oCollisionMap.m_vTexels[index + 3 * j + 2] = r;
			}
		}
	}
}

void CCollisionManager::GetPositionFromCellCoord(int row, int column, float& x, float& y)
{
	int rowCount, columnCount;
	ComputeRowAndColumnCount(rowCount, columnCount);
	x = (0.5f + (float)column - (float)columnCount / 2.f) * m_fGridCellSize;
	y = (0.5f + (float)row - (float)rowCount / 2.f) * m_fGridCellSize;
}

void CCollisionManager::GetCellCoordFromPosition(float x, float y, int& row, int& column)
{
	row = (int)( (x + m_fGroundWidth / 2) / m_fGridCellSize);
	column = (int)( (y + m_fGroundHeight / 2) / m_fGridCellSize);
}

void CCollisionManager::ComputeRowAndColumnCount(int& rowCount, int& columnCount)
{
	rowCount = m_fGroundHeight / m_fGridCellSize;
	columnCount = m_fGroundWidth / m_fGridCellSize;
}

void CCollisionManager::ConvertLinearToCoord(int pixelNumber, int& x, int& y)
{
	x = pixelNumber / 3 - pixelNumber / (3 * m_oCollisionMap.m_nWidth) * m_oCollisionMap.m_nWidth;
	y = pixelNumber / (3 * m_oCollisionMap.m_nWidth);
}

void CCollisionManager::EnableHMHack(bool enable)
{
	//m_bEnableHMHack2 = !enable;
	m_bEnableHMHack = enable;
}

void CCollisionManager::EnableHMHack2(bool enable)
{
	//m_bEnableHMHack = !enable;
	m_bEnableHMHack2 = enable;
}

bool CCollisionManager::TestCellObstacle(int row, int column)
{
	float x0, y0, x1, y1;
	GetPositionFromCellCoord(row, column, x0, y0);
	GetPositionFromCellCoord(row + 1, column + 1, x1, y1);

	float mapX0 = WorldToPixel(x0);
	float mapX1 = WorldToPixel(x1);

	int subdivisionCount = 30;
	int mapBoxSize = mapX1 - mapX0;
	float fmapBoxSize = mapX1 - mapX0;
	int rowCount = m_oCollisionMap.m_nHeight / mapBoxSize;
	int columnCount = m_oCollisionMap.m_nWidth / mapBoxSize;

	float fx = (float)column * fmapBoxSize;
	int ix = (int)fx;
	float fy = (float)row * fmapBoxSize;
	int iy = (int)fy;

	bool isObstacle = false;
	for (int i = 0; i < mapBoxSize; i++) {
		int index = 3 * (ix + i * m_oCollisionMap.m_nWidth + iy * m_oCollisionMap.m_nWidth);
		int fillSize = 3 * mapBoxSize;
		if (column == columnCount) {
			fillSize = (m_oCollisionMap.m_nWidth * 3) - column * 3 * mapBoxSize;
		}

		for (int j = 0; j < fillSize / 3; j++) {
			char rgb[3];
			char obstacle[3] = { 0, 0, 255 };
			memcpy(rgb, &m_oCollisionMap.m_vTexels[index + j * 3], 3);

			int c = memcmp(obstacle, rgb, 3);
			if (c == 0)
				return true;
		}
	}
	return false;
}

float CCollisionManager::WorldToPixel(float worldLenght)
{
	float ret;
	IBox* pBox = m_pGround->GetBBox();
	if (pBox->GetDimension().m_x <= pBox->GetDimension().m_z * m_fScreenRatio)
		ret = (worldLenght * (float)m_oCollisionMap.m_nHeight) / pBox->GetDimension().m_z;
	else
		ret = worldLenght * (float)m_oCollisionMap.m_nWidth / pBox->GetDimension().m_x;
	return ret;
}

void CCollisionManager::MarkObstacles(IEntityManager* pEntityManager)
{
	float originX = -m_fGroundWidth / 2;
	float originY = -m_fGroundHeight / 2;
	int firstRow = 0, firstColumn = 0;
	int rowCount, columnCount;
	GetCellCoordFromPosition(originX, originY, firstRow, firstColumn);
	ComputeRowAndColumnCount(rowCount, columnCount);
	
	m_pCollisionGrid = new char*[rowCount];
	for (int iRow = 0; iRow < rowCount; iRow++) {
		m_pCollisionGrid[iRow] = new char[columnCount];
	}

	for (int iRow = firstRow; iRow < firstRow + rowCount; iRow++) {
		for (int iColumn = firstColumn; iColumn < firstColumn + columnCount; iColumn++) {
			bool obstacle = TestCellObstacle(iRow, iColumn);
			if (obstacle) {
				m_pCollisionGrid[iRow][iColumn] = 'o';
				MarkBox(iRow, iColumn, 1, 0, 0, pEntityManager);
				MarkMapBox(iRow, iColumn, 255, 0, 0);
			}
			else {
				m_pCollisionGrid[iRow][iColumn] = 'f';
			}
		}
	}
	m_oLoaderManager.Export("CollisionFinal_terrain.bmp", m_oCollisionMap);
}

void CCollisionManager::FindPath(float fromX, float fromY, float toX, float toY, IEntityManager* pEntityManager)
{
	MarkObstacles(pEntityManager);
}

void CCollisionManager::Test(IEntityManager* pEntityManager)
{
	float fromX = 2000.f;
	float fromY = 0.f;
	float toX = 1900.f;
	float toY = 6000.f;
	FindPath(fromX, fromY, toX, toY, pEntityManager);
}

void CCollisionManager::Test2()
{
	for (vector<IEntity*>::iterator it = m_vGridElements.begin(); it != m_vGridElements.end(); it++) {
		(*it)->Unlink();
		delete (*it);
	}
	m_vGridElements.clear();
}

void CCollisionManager::SetEntityManager(IEntityManager* pEntityManager)
{
	m_pEntityManager = pEntityManager;
}

extern "C" _declspec(dllexport) CCollisionManager* CreateCollisionManager(EEInterface& oInterface)
{
	return new CCollisionManager(oInterface);
}