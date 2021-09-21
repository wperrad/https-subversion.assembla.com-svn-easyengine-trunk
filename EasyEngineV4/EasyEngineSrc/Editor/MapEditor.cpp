#include "IInputManager.h"
#include "MapEditor.h"
#include "ICameraManager.h"
#include "IRenderer.h"
#include "ICamera.h"
#include "IGeometry.h"
#include "Interface.h"
#include "IEntity.h"
#include "ICollisionManager.h"
#include "IHud.h"
#include "ILoader.h"
#include "IFileSystem.h"
#include "EEPlugin.h"

IEventDispatcher::TKeyEvent	CMapEditor::m_eLastKeyEvent = IEventDispatcher::TKeyEvent::T_KEYUP;

CMapEditor::CMapEditor(EEInterface& oInterface) :
IMapEditor(oInterface),
m_oInputManager(*static_cast<IInputManager*>(oInterface.GetPlugin("InputManager"))),
m_oCameraManager(*static_cast<ICameraManager*>(oInterface.GetPlugin("CameraManager"))),
m_oRenderer(*static_cast<IRenderer*>(oInterface.GetPlugin("Renderer"))),
m_oEntityManager(*static_cast<IEntityManager*>(oInterface.GetPlugin("EntityManager"))),
m_oGeometryManager(*static_cast<IGeometryManager*>(oInterface.GetPlugin("GeometryManager"))),
m_oLoaderManager(*static_cast<ILoaderManager*>(oInterface.GetPlugin("LoaderManager"))),
m_oRessourceManager(*static_cast<IRessourceManager*>(oInterface.GetPlugin("RessourceManager"))),
m_oCollisionManager(*static_cast<ICollisionManager*>(oInterface.GetPlugin("CollisionManager"))),
m_oHud(*static_cast<IHud*>(oInterface.GetPlugin("HUD"))),
m_oFileSystem(*static_cast<IFileSystem*>(oInterface.GetPlugin("FileSystem"))),
m_bEditionMode(false),
m_bAddEntityMode(false),
m_pCurrentAddedEntity(NULL),
m_fPlanHeight(3000.f),
m_pSelectedEntity(NULL),
m_fGroundAdaptationHeight(0.f),
m_nHudX(800),
m_nHudY(150),
m_pScene(nullptr),
m_pHeightMap(nullptr),
m_nHudLineHeight(15),
m_bDisplayPickingRay(false)
{
	IEventDispatcher* pEventDispatcher = static_cast<IEventDispatcher*>(oInterface.GetPlugin("EventDispatcher"));
	pEventDispatcher->AbonneToMouseEvent(this, OnMouseEventCallback);
	pEventDispatcher->AbonneToKeyEvent(this, OnKeyEventCallback);
	pEventDispatcher->AbonneToEntityEvent(this, OnSceneLoadRessource);
	string sSceneFileName;
	ISceneManager* pSceneManager = static_cast<ISceneManager*>(oInterface.GetPlugin("SceneManager"));
	m_pScene = pSceneManager->GetScene("Game");
	m_sTmpFolder = "levels/tmp";
	m_sTmpAdaptedHeightMapFileName = "/" + m_sTmpFolder + "/HMA_tmp.bmp";
}

void CMapEditor::SetEditionMode(bool bEditionMode)
{
	m_bEditionMode = bEditionMode;
	m_oInputManager.SetEditionMode(m_bEditionMode);
	if (m_bEditionMode) {
		ICamera* pFreeCamera = m_oCameraManager.GetCameraFromType(ICameraManager::T_FREE_CAMERA);
		m_oCameraManager.SetActiveCamera(pFreeCamera);
	}
	else {
		ICamera* pLinkCamera = m_oCameraManager.GetCameraFromType(ICameraManager::T_LINKED_CAMERA);
		m_oCameraManager.SetActiveCamera(pLinkCamera);
	}

	m_oHud.Clear();
	if (m_bEditionMode) {
		m_oHud.Print("Mode Edition : Vous pouvez importer un modele en utilisant la commande 'SpawnEntity'", m_nHudX, m_nHudY);
		m_oHud.Print("Vous pouvez sauvegarder le niveau grace la commande 'SaveLavel(levelName)'", m_nHudX, m_nHudY + m_nHudLineHeight);
	}
}

void CMapEditor::OnMouseEventCallback(CPlugin* plugin, IEventDispatcher::TMouseEvent e, int x, int y)
{
	CMapEditor* pEditor = dynamic_cast<CMapEditor*>(plugin);
	if (pEditor->m_bEditionMode) {
		if (e == IEventDispatcher::TMouseEvent::T_LBUTTONDOWN)
			pEditor->OnLeftMouseDown(x, y);
		else if (e == IEventDispatcher::TMouseEvent::T_RBUTTONDOWN)
			pEditor->m_oInputManager.SetEditionMode(false);
		else if (e == IEventDispatcher::TMouseEvent::T_RBUTTONUP) {
			pEditor->m_oInputManager.SetEditionMode(true);
		}
		else if (e == IEventDispatcher::TMouseEvent::T_MOVE) {
			if (pEditor->m_pCurrentAddedEntity) {
				CVector intersect;
				pEditor->GetRayPlanIntersection(x, y, pEditor->m_fPlanHeight, intersect);
				pEditor->m_pCurrentAddedEntity->SetLocalPosition(intersect.m_x, pEditor->m_fPlanHeight, intersect.m_z);
			}
		}
	}
}

void CMapEditor::OnLeftMouseDown(int x, int y)
{
	if (m_pCurrentAddedEntity) {
		m_pCurrentAddedEntity->SetWeight(1.f);
		m_pScene->UpdateMapEntities();
		ManageGroundAdaptation(-m_fGroundAdaptationHeight);
		m_pCurrentAddedEntity = NULL;
	}
	else {
		CVector intersect;
		SelectEntity(x, y);
		if (m_pSelectedEntity) {
			m_pCurrentAddedEntity = m_pSelectedEntity;
			CVector pos;
			m_pCurrentAddedEntity->GetWorldPosition(pos);
			m_pCurrentAddedEntity->SetLocalPosition(pos.m_x, m_fPlanHeight, pos.m_z);
			m_pCurrentAddedEntity->SetWeight(0.f);
			m_pCurrentAddedEntity->Update();
			IBox* pBox = dynamic_cast<IBox*>(m_pSelectedEntity->GetBoundingGeometry());
			string originalRessourceName;
			m_pScene->GetOriginalSceneFileName(originalRessourceName);
			if (!m_pHeightMap)
				m_pHeightMap = m_oCollisionManager.GetHeightMap(m_pScene->GetCurrentHeightMapIndex());
			m_pHeightMap->RestoreHeightMap(m_pSelectedEntity->GetWorldMatrix(), pBox->GetDimension(), originalRessourceName);
			UpdateGround();
		}
	}
}

void CMapEditor::OnKeyEventCallback(CPlugin* plugin, IEventDispatcher::TKeyEvent e, int key)
{
	if (e == IEventDispatcher::T_KEYUP) {
		CMapEditor* pEditor = static_cast<CMapEditor*>(plugin);
		if (pEditor->m_bEditionMode) {
			if (m_eLastKeyEvent == IEventDispatcher::T_KEYDOWN) {
				if (pEditor->m_pSelectedEntity) {
					if (key == VK_DELETE) {
						pEditor->m_pSelectedEntity->Unlink();
						pEditor->m_pSelectedEntity = nullptr;
					}
				}
				else if (pEditor->m_pCurrentAddedEntity) {
					if (key == VK_DELETE) {
						pEditor->m_pCurrentAddedEntity->Unlink();
						pEditor->m_pCurrentAddedEntity = nullptr;
					}
					else if (key == VK_LEFT) {
						pEditor->m_pCurrentAddedEntity->Yaw(1.f);
					}
					else if (key == VK_RIGHT) {
						pEditor->m_pCurrentAddedEntity->Yaw(-1.f);
					}
				}
			}
		}
	}
	m_eLastKeyEvent = e;
}

void CMapEditor::ManageGroundAdaptation(float deltaHeight)
{
	m_fGroundAdaptationHeight = deltaHeight;
	AdaptGroundToEntity(m_pCurrentAddedEntity);
	
}

void CMapEditor::RayCast(int x, int y, CVector& p1, CVector& ray)
{
	CMatrix P, Pinv;
	m_oRenderer.GetProjectionMatrix(P);
	P.GetInverse(Pinv);
	CMatrix Vinv;
	m_oCameraManager.GetActiveCamera()->GetWorldMatrix(Vinv);

	int width, height;
	m_oRenderer.GetResolution(width, height);
	float logicalx = (2.f * (float)x / (float)width) - 1.f;
	float logicaly = 1.f - (2.f * (float)y / (float)height);

	CVector ray_nds(logicalx, logicaly, 1.f, 1.f);
	CVector ray_clip = CVector(ray_nds.m_x, ray_nds.m_y, -1.0, 1.0);

	CVector ray_eye = Pinv * ray_clip;
	ray_eye = CVector(ray_eye.m_x, ray_eye.m_y, -1.f, 0.f);

	ray = Vinv * ray_eye;
	ray.Normalize();

	Vinv.GetPosition(p1);
}

bool CMapEditor::IsIntersect(const CVector& linePt1, const CVector& linePt2, const CVector& M, float radius) const
{
	CVector P12 = linePt2 - linePt1;
	CVector P1M = M - linePt1;
	float alpha = acosf( (P12 * P1M) / (P12.Norm() * P1M.Norm()) );
	float d = P1M.Norm() * sinf(alpha);
	return d < radius;
}

void CMapEditor::GetRayPlanIntersection(int x, int y, float h, CVector& intersect)
{
	CVector camPos, ray_wor;
	RayCast(x, y, camPos, ray_wor);

#ifdef DEBUG_TEST_PLANE
	IEntity* pLineEntity = m_oEntityManager.CreateLineEntity(camPos, ray_wor);
	pLineEntity->Link(m_pScene);
#endif // DEBUG_TEST_PLANE

	IMesh* pMesh = static_cast<IMesh*>(m_pScene->GetRessource());
	const CVector& d = pMesh->GetBBox()->GetDimension();
	IQuad* quad = m_oGeometryManager.CreateQuad(d.m_x, d.m_z);
	CMatrix quadTM;
	quadTM.SetPosition(0, h, 0);
	quad->SetTM(quadTM);

	CVector B = camPos + ray_wor * 50000.f;
	B.m_w = 1.f;
	quad->GetLineIntersection(camPos, B, intersect);
	delete quad;

#ifdef DEBUG_TEST_PLANE
	IEntity* pSphere = m_oEntityManager.CreateSphere(100.f);
	pSphere->Link(m_pScene);
	pSphere->SetLocalPosition(intersect);

	if (!m_pQuadEntity) {
		m_pQuadEntity = m_oEntityManager.CreateQuad(d.m_x, d.m_z);
		m_pQuadEntity->Link(m_pScene);
		m_pQuadEntity->SetLocalPosition(0.f, h, 0.f);
	}
#endif // DEBUG_TEST_PLANE
}

void CMapEditor::SelectEntity(int x, int y)
{
	CVector camPos, ray_wor;
	RayCast(x, y, camPos, ray_wor);
	CVector B = camPos + ray_wor * 50000.f;
	B.m_w = 1.f;
	
	if (m_bDisplayPickingRay) {
		IEntity* pLineEntity = m_oEntityManager.CreateLineEntity(camPos, B);
		pLineEntity->Link(m_pScene);
	}

	IEntity* pSelectedEntity = NULL;
	IScene* pScene = dynamic_cast<IScene*>(m_pScene);
	vector<IEntity*> entities;
	pScene->CollectMapEntities(entities);
	for (int i = 0; i < entities.size(); i++) {
		IEntity* pEntity = entities[i];		
		CVector pos;
		pEntity->GetWorldPosition(pos);
		if (IsIntersect(camPos, B, pos, pEntity->GetBoundingSphereRadius())) {
			float lastDistanceToCam = 999999999.f;
			CVector lastPos, currentPos;
			if (pSelectedEntity) {
				pSelectedEntity->GetWorldPosition(lastPos);
				lastDistanceToCam = (lastPos - camPos).Norm();
			}
			pEntity->GetWorldPosition(currentPos);
			float newDistanceToCam = (currentPos - camPos).Norm();
			if (newDistanceToCam < lastDistanceToCam) {
				pSelectedEntity = pEntity;
			}
		}
	}
	if (pSelectedEntity) {
		if (m_pSelectedEntity)
			m_pSelectedEntity->DrawBoundingBox(false);
		pSelectedEntity->DrawBoundingBox(true);
		m_pSelectedEntity = pSelectedEntity;
		DisplayLocalRepere();
	}
	else if(m_pSelectedEntity) {
		m_pSelectedEntity->DrawBoundingBox(false);
	}
}

void CMapEditor::DisplayLocalRepere()
{
	return;
	CVector position;
	m_pSelectedEntity->GetWorldPosition(position);
	IEntity* x = m_oEntityManager.CreateCylinder(500, 10000);
}

void CMapEditor::DisplayPickingRay(bool enable)
{
	m_bDisplayPickingRay = enable;
}

void CMapEditor::AdaptGroundToEntity(IEntity* pEntity)
{
	if (pEntity) {
		IBox* pBox = dynamic_cast<IBox*>(pEntity->GetBoundingGeometry());
		if (pBox) {
			CVector dim = pBox->GetDimension();
			CVector pos;
			pEntity->GetWorldPosition(pos);
			if (m_oAnimatableMeshData.m_vMeshes.size() > 0) {
				ILoader::CMeshInfos& mi = m_oAnimatableMeshData.m_vMeshes[0];
				float hMin = 99999999.f;
				for (int i = 0; i < mi.m_vVertex.size() / 3; i++) {
					float x = mi.m_vVertex[i * 3];
					float y = mi.m_vVertex[i * 3 + 1];
					float z = mi.m_vVertex[i * 3 + 2];

					if ((x >= (pos.m_x - dim.m_x)) && (x <= (pos.m_x + dim.m_x)) &&
						(z >= (pos.m_z - dim.m_z)) && (z <= (pos.m_z + dim.m_z))) {
						if (hMin > y)
							hMin = y;
					}
				}
				hMin += m_fGroundAdaptationHeight;

				for (int i = 0; i < mi.m_vVertex.size() / 3; i++) {
					float x = mi.m_vVertex[i * 3];
					float z = mi.m_vVertex[i * 3 + 2];

					if ((x >= (pos.m_x - dim.m_x)) && (x <= (pos.m_x + dim.m_x)) &&
						(z >= (pos.m_z - dim.m_z)) && (z <= (pos.m_z + dim.m_z))) {
						mi.m_vVertex[i * 3 + 1] = hMin;
					}
				}
			}
			else {

				if (!m_pHeightMap)
					m_pHeightMap = m_oCollisionManager.GetHeightMap(m_pScene->GetCurrentHeightMapIndex());
				int idx = m_oHud.Print("Adaptation du terrain en cours", m_nHudX, m_nHudY + m_oHud.GetLineCount() * m_nHudLineHeight);

				static bool multiThreaded = false;
				if (multiThreaded) {
					DWORD threadId = 0;
					AdaptGroundThreadStruct* params = new AdaptGroundThreadStruct;
					params->pos = pos;
					params->dim = dim;
					params->editor = this;
					params->hudMsgIdx = idx;
					params->adaptationHeight = m_fGroundAdaptationHeight;
					CreateThread(nullptr, 0, AdaptGround, params, 0, &threadId);
				}
				else {
					m_pHeightMap->AdaptGroundMapToModelOptimized(pEntity->GetWorldMatrix(), pBox->GetDimension(), m_fGroundAdaptationHeight);
					m_oHud.RemoveText(idx);
					m_oHud.Print("Adaptation terminee", m_nHudX, m_nHudY + m_oHud.GetLineCount() * m_nHudLineHeight);
					UpdateGround();
				}
			}
		}
	}
}

DWORD CMapEditor::AdaptGround(void* params)
{
	AdaptGroundThreadStruct* s = (AdaptGroundThreadStruct*)params;
	s->editor->m_pHeightMap->AdaptGroundMapToModel(s->pos, s->dim, s->adaptationHeight);
	s->editor->OnEndAdaptGround(s->hudMsgIdx);
	delete s;
	return 0;
}

void CMapEditor::OnEndAdaptGround(int hudMsgIdx)
{
	m_oHud.RemoveText(hudMsgIdx);
	m_oHud.Print("Adaptation terminee", m_nHudX, m_nHudY + m_oHud.GetLineCount() * m_nHudLineHeight);
}

void CMapEditor::UpdateGround()
{
	if ( m_oAnimatableMeshData.m_vMeshes.empty() && m_pHeightMap) {
		string root;
		m_oFileSystem.GetLastDirectory(root);
		WIN32_FIND_DATA wfd;
		string tmpFolder = "/" + m_sTmpFolder;
		string tmpPath = root + tmpFolder;
		HANDLE hLevelFolder = m_oFileSystem.FindFirstFile_EE(tmpPath, wfd);
		bool folderCreated = hLevelFolder != INVALID_HANDLE_VALUE;
		if ((!folderCreated) || !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			folderCreated = CreateDirectoryA(tmpPath.c_str(), nullptr);
		}
		if (!folderCreated) {
			m_oHud.Print(string("Erreur, impossible de creer le dossier '") + tmpPath + "'", m_nHudX, m_nHudY + m_oHud.GetLineCount() * m_nHudLineHeight);
			return;
		}
	
		string originalRessourceName;
		m_pScene->GetOriginalSceneFileName(originalRessourceName);
		if (originalRessourceName.empty()) {
			m_pHeightMap->GetFileName(originalRessourceName);
			m_pScene->SetOriginalSceneFileName(originalRessourceName);
		}

		string sGroundFileName;
		m_pScene->GetRessource()->GetFileName(sGroundFileName);
		string srcPath = root + sGroundFileName;

		string sRessourceFileName;
		m_pScene->GetRessource()->GetFileName(sRessourceFileName);
		string levelPath = sRessourceFileName.substr(0, sRessourceFileName.find_last_of("/") + 1);
		string destPath = root + "/" + m_sTmpFolder + "/ground.bme";
		if(srcPath != destPath)
			CopyFileA(srcPath.c_str(), destPath.c_str(), TRUE);

		m_pHeightMap->Save(m_sTmpAdaptedHeightMapFileName);
		m_pScene->SetRessource(m_sTmpAdaptedHeightMapFileName);
	}
}

bool CMapEditor::CreateLevelFolderIfNotExists(string levelName, string& levelFolder)
{
	string root;
	m_oFileSystem.GetLastDirectory(root);
	WIN32_FIND_DATA wfd;
	string relativeFolderPath = "/levels";
	levelFolder = root + relativeFolderPath;
	HANDLE hLevelFolder = m_oFileSystem.FindFirstFile_EE(levelFolder, wfd);
	bool folderCreated = hLevelFolder != INVALID_HANDLE_VALUE;
	if ((!folderCreated) || !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
		folderCreated = CreateDirectoryA(levelFolder.c_str(), nullptr);
	}
	if (!folderCreated) {
		m_oHud.Print(string("Erreur, impossible de creer le dossier '") + levelFolder + "'", m_nHudX, m_nHudY + m_oHud.GetLineCount() * m_nHudLineHeight);
		return false;
	}

	levelFolder += "/" + levelName;
	hLevelFolder = m_oFileSystem.FindFirstFile_EE(levelFolder, wfd);
	folderCreated = hLevelFolder != INVALID_HANDLE_VALUE;
	if ((!folderCreated) || !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
		folderCreated = CreateDirectoryA(levelFolder.c_str(), nullptr);
	}
	if (!folderCreated) {
		m_oHud.Print(string("Erreur, impossible de creer le dossier '") + levelFolder + "'", m_nHudX, m_nHudY + m_oHud.GetLineCount() * m_nHudLineHeight);
		return false;
	}
	return true;
}

void CMapEditor::Save(string mapName)
{
	if (m_bEditionMode) {
		string levelFolder;
		if (CreateLevelFolderIfNotExists(mapName, levelFolder)) {
			levelFolder += "/";
			string originalRessourceName;
			if (m_oAnimatableMeshData.m_vMeshes.size() > 0) {
				string sFileName = mapName + ".bme";
				m_oLoaderManager.Export(sFileName, m_oAnimatableMeshData);
				m_oCollisionManager.CreateHeightMap(sFileName);
				m_pScene->GetOriginalSceneFileName(originalRessourceName);
				m_pScene->SetOriginalSceneFileName(originalRessourceName);
				m_pScene->SetRessource(sFileName);
			}
			else {
				string root;
				m_oFileSystem.GetLastDirectory(root);
				string srcPath = root + m_sTmpAdaptedHeightMapFileName;
				string newHMFile = "levels/" + mapName + "/HMA_" + mapName + ".bmp";
				string destPath = levelFolder + "HMA_" + mapName + ".bmp";
				m_pScene->SetHMFile("/" + newHMFile);
				if (!MoveFileA(srcPath.c_str(), destPath.c_str())) {
					if (!m_pHeightMap)
						m_pHeightMap = m_oCollisionManager.GetHeightMap(m_pScene->GetCurrentHeightMapIndex());
					m_pHeightMap->Save(newHMFile);
				}
				srcPath = root + "/" + m_sTmpFolder + "/ground.bme";;
				destPath = levelFolder + "ground.bme";
				if (!MoveFileA(srcPath.c_str(), destPath.c_str())) {
					string sGroundFileName;
					m_pScene->GetRessource()->GetFileName(sGroundFileName);
					srcPath = root + sGroundFileName;
					CopyFileA(srcPath.c_str(), destPath.c_str(), FALSE);
				}
			}
			SaveMap(levelFolder + mapName + ".bse");
		}
	}
	else {
		CEException e("You have to enable MapEditionMode to be able to save the map.");
		throw e;
	}
}

void CMapEditor::SaveMap(string sFileName)
{
	ILoader::CSceneInfos si;
	m_pScene->GetInfos(si);
	ClearCharacters(si.m_vObject);
	m_oLoaderManager.Export(sFileName, si);
}

void CMapEditor::Load(string sFileName)
{
	SetEditionMode(true);
	ILoader::CSceneInfos si;
	m_oLoaderManager.Load(sFileName, si);
	m_pScene->Load(si);
	ICamera* pCamera = m_oCameraManager.GetCameraFromType(ICameraManager::TCameraType::T_FREE_CAMERA);
	m_oEntityManager.AddEntity(pCamera, "FreeCamera");
	m_oCameraManager.SetActiveCamera(pCamera);
	IBox* pBBox = dynamic_cast<IBox*>(m_pScene->GetBoundingGeometry());
	if (pBBox)
		pCamera->Move(0, -60.f, 0, 0, 0, pBBox->GetDimension().m_x / 8.f);
}

void CMapEditor::ClearCharacters(vector<ILoader::CObjectInfos*>& objects)
{
	vector<ILoader::CObjectInfos*>::iterator itObject = objects.begin();
	while (itObject != objects.end()) {
		ILoader::CAnimatedEntityInfos* pChildCharacter = dynamic_cast<ILoader::CAnimatedEntityInfos*>(*itObject);
		if (pChildCharacter)
			itObject = objects.erase(itObject);
		else {
			ILoader::CEntityInfos* pEntityInfos = dynamic_cast<ILoader::CEntityInfos*>(*itObject);
			if (pEntityInfos)
				ClearCharacters(pEntityInfos->m_vSubEntityInfos);
			itObject++;
		}
	}
}

void CMapEditor::SetGroundAdaptationHeight(float fHeight)
{
	m_fGroundAdaptationHeight = fHeight;
}

string CMapEditor::GetName()
{
	return "Editor";
}

void CMapEditor::SpawnEntity(string sEntityFileName)
{
	m_bAddEntityMode = true;
	m_pCurrentAddedEntity = m_oEntityManager.CreateEntity(sEntityFileName, "");
	m_pCurrentAddedEntity->Link(m_pScene);
	m_pCurrentAddedEntity->SetLocalPosition(0, m_fPlanHeight, 0);
	m_pCurrentAddedEntity->Update();
}

void CMapEditor::OnSceneLoadRessource(CPlugin* plugin, IEventDispatcher::TEntityEvent e, IEntity* pEntity)
{
	if (e == IEventDispatcher::TEntityEvent::T_LOAD_RESSOURCE) {
		CMapEditor* pEditor = static_cast<CMapEditor*>(plugin);
		string sSceneFileName;
		pEditor->m_pScene->GetRessource()->GetFileName(sSceneFileName);
		pEditor->m_oLoaderManager.Load(sSceneFileName, pEditor->m_oAnimatableMeshData);
	}
}

