#include "WorldEditor.h"
#include "IEntity.h"
#include "Interface.h"
#include "IFileSystem.h"
#include "ICamera.h"
#include "IGeometry.h"
#include "IConsole.h"
#include "EditorManager.h"


CWorldEditor::CWorldEditor(EEInterface& oInterface, ICameraManager::TCameraType cameraType) :
	CPlugin(nullptr, ""),
	ISpawnableEditor(oInterface),
	IWorldEditor(oInterface),
	CSpawnableEditor(oInterface, cameraType),
	m_oSceneManager(static_cast<ISceneManager&>(*oInterface.GetPlugin("SceneManager"))),
	m_oFileSystem(static_cast<IFileSystem&>(*oInterface.GetPlugin("FileSystem")))
{
	m_pScene = m_oSceneManager.GetScene("Game");
}

void CWorldEditor::HandleMapLoaded(string sMapName)
{
	m_mMaps[sMapName] = CVector();
}

void CWorldEditor::ClearWorld()
{
	m_pScene->Clear();
}

void CWorldEditor::OnEntityAdded()
{
	map<string, vector<pair<IEntity*, CMatrix>>>::iterator itEntity = m_mEntityMatrices.find(m_pCurrentAddedEntity->GetName());
	if (itEntity != m_mEntityMatrices.end()) {
		vector<pair<IEntity*, CMatrix>>& vMatrices = itEntity->second;
		vMatrices.back().first = m_pCurrentAddedEntity;
		vMatrices.back().second = m_pCurrentAddedEntity->GetWorldMatrix();
	}
}

float CWorldEditor::GetPlanHeight()
{
	if (m_pCurrentAddedEntity) {
		ICamera* pCamera = m_oCameraManager.GetActiveCamera();
		CVector camPos;
		pCamera->GetWorldPosition(camPos);
		IBox* pBox = dynamic_cast<IBox*>(m_pCurrentAddedEntity->GetBoundingGeometry());
		if (pBox)
			return camPos.m_y - 200.f;
	}
	return 0.f;
}

void CWorldEditor::OnEntitySelected()
{

}

void CWorldEditor::OnEntityRemoved(IEntity* pEntity)
{
	map<string, vector<pair<IEntity*, CMatrix>>>::iterator itEntity = m_mEntityMatrices.find(pEntity->GetName());
	if (itEntity != m_mEntityMatrices.end()) {
		vector<pair<IEntity*, CMatrix>>& vMatrices = itEntity->second;
		for (vector<pair<IEntity*, CMatrix>>::iterator itMat = vMatrices.begin(); itMat != vMatrices.end(); itMat++)
			if (itMat->first == pEntity)
				itMat = vMatrices.erase(itMat);
	}
	else {
		map<string, CMatrix>::iterator itCharacter = m_mCharacterMatrices.find(pEntity->GetName());
		m_mCharacterMatrices.erase(itCharacter);
	}
}

string CWorldEditor::GetName()
{
	return "WorldEditor";
}

void CWorldEditor::Load(string fileName)
{
	m_pEditorManager->CloseAllEditorButThis(this);
	ClearEntities();
	if (fileName.empty())
		GetRelativeDatabasePath(fileName);

	CBinaryFileStorage fs;
	if (fs.OpenFile(fileName, IFileStorage::TOpenMode::eRead)) {
		int mapCount = 0;
		fs >> mapCount;
		for (int i = 0; i < mapCount; i++) {
			string mapName;
			CVector pos;
			fs >> mapName >> pos;
			m_mMaps[mapName] = pos;
		}
		if (m_mMaps.size() > 0) {
			string root;
			m_oFileSystem.GetLastDirectory(root);
			IMapEditor* pMapEditor = dynamic_cast<IMapEditor*>(m_pEditorManager->GetEditor(IEditor::Type::eMap));
			m_pScene->HandleLoadingComplete(HandleSceneLoadingComplete, this);
			pMapEditor->Load(m_mMaps.begin()->first);
		}

		int characterCount = 0;
		fs >> characterCount;
		for (int i = 0; i < characterCount; i++) {
			string id;
			CMatrix tm;
			fs >> id >> tm;
			m_mCharacterMatrices[id] = tm;
		}
		int entityCount = 0;
		fs >> entityCount;
		for (int i = 0; i < entityCount; i++) {
			string sFileName;
			int instanceCount = 0;
			fs >> sFileName >> instanceCount;
			for (int iInstance = 0; iInstance < instanceCount; iInstance++) {
				CMatrix tm;
				fs >> tm;
				m_mEntityMatrices[sFileName].push_back(pair<IEntity*, CMatrix>(nullptr, tm));
			}
		}
	}
	m_pEditorCamera->Link(m_pScene);
}

void CWorldEditor::Save(string fileName)
{
	if (fileName.empty())
		GetRelativeDatabasePath(fileName);
	CopyFile(fileName.c_str(), (fileName + ".bak").c_str(), FALSE);

	CBinaryFileStorage fs;
	if (fs.OpenFile(fileName, IFileStorage::TOpenMode::eWrite)) {
		fs << (int)m_mMaps.size();
		for (map<string, CVector>::iterator itMap = m_mMaps.begin(); itMap != m_mMaps.end(); itMap++)
			fs << itMap->first << itMap->second;
		fs << (int)m_mCharacterMatrices.size();
		for (map<string, CMatrix>::iterator it = m_mCharacterMatrices.begin(); it != m_mCharacterMatrices.end(); it++) {
			IEntity* pEntity = m_oEntityManager.GetEntity(it->first);
			if (pEntity)
				fs << it->first << pEntity->GetWorldMatrix();
			else {
				ostringstream oss;
				oss << "Erreur : CWorldEditor::Save() -> Personnage " << it->first << " introuvable dans l'EntityManager";
				throw CEException(oss.str());
			}
		}
		fs << (int)m_mEntityMatrices.size();
		for (map<string, vector<pair<IEntity*, CMatrix>>>::iterator it = m_mEntityMatrices.begin(); it != m_mEntityMatrices.end(); it++) {
			fs << it->first;
			fs << (int)it->second.size();
			for(int iMatrix = 0; iMatrix < it->second.size(); iMatrix++) {
				if(it->second[iMatrix].first)
					fs << it->second[iMatrix].first->GetWorldMatrix();
			}
		}
	}
}

void CWorldEditor::SpawnEntity(string sFileName)
{
	if(!m_bEditionMode)
		SetEditionMode(true);

	string relativePath = string("Meshes/") + sFileName;
	m_pCurrentAddedEntity = m_oEntityManager.CreateObject(relativePath);
	m_pCurrentAddedEntity->SetName(sFileName);
	m_oEntityManager.AddEntity(m_pCurrentAddedEntity, relativePath);
	InitSpawnedEntity();
	CMatrix m;
	m_mEntityMatrices[sFileName].push_back(pair<IEntity*, CMatrix>(nullptr, m));
	m_vEntities.push_back(m_pCurrentAddedEntity);
}

void CWorldEditor::RemoveCharacter(string sID)
{
	m_mCharacterMatrices.erase(sID);
	IEntity* pCharacter = m_oEntityManager.GetEntity(sID);
	vector<IEntity*>::iterator itCharacter = std::find(m_vEntities.begin(), m_vEntities.end(), pCharacter);
	m_vEntities.erase(itCharacter);
}

void CWorldEditor::SpawnCharacter(string sID)
{
	if (!m_bEditionMode)
		SetEditionMode(true);

	m_pCurrentAddedEntity = m_oEntityManager.BuildCharacterFromDatabase(sID, m_pScene);
	if (!m_pCurrentAddedEntity) {
		ostringstream oss;
		oss << "Erreur : Personnage " << sID << " introuvable dans la base de donnees des personnages.";
		throw CEException(oss.str());
	}
	m_oEntityManager.AddEntity(m_pCurrentAddedEntity, sID);
	InitSpawnedEntity();
	CMatrix m;
	m_mCharacterMatrices[sID] = m;
	m_vEntities.push_back(m_pCurrentAddedEntity);
	m_oCameraManager.SetActiveCamera(m_pEditorCamera);
}

void CWorldEditor::SetEditionMode(bool bEditionMode)
{
	if (m_bEditionMode != bEditionMode) {
		CEditor::SetEditionMode(bEditionMode);
		if (bEditionMode) {
			m_pScene->Clear();
			Load("");
		}
		else {
			// Ask to save world
		}
	}
}

void CWorldEditor::CollectSelectableEntity(vector<IEntity*>& entities)
{
	entities = m_vEntities;
}

void CWorldEditor::Edit(string id)
{
	SetEditionMode(true);
}

void CWorldEditor::GetRelativeDatabasePath(string& path)
{
	string root;
	m_oFileSystem.GetLastDirectory(root);
	path = root + "/" + m_sDatabaseFileName;
}

void CWorldEditor::ClearEntities()
{
	m_pScene->Clear();
	m_vEntities.clear();
	m_mCharacterMatrices.clear();
	m_mEntityMatrices.clear();
	m_oEntityManager.Clear();
}

void CWorldEditor::OnSceneLoaded()
{
	for (map<string, CMatrix>::iterator itCharacter = m_mCharacterMatrices.begin(); itCharacter != m_mCharacterMatrices.end(); itCharacter++) {
		IEntity* pEntity = m_oEntityManager.BuildCharacterFromDatabase(itCharacter->first, m_pScene);
		if (pEntity) {
			pEntity->SetLocalMatrix(itCharacter->second);
			pEntity->SetWeight(1);
			m_vEntities.push_back(pEntity);
		}
	}
	for (map<string, vector<pair<IEntity*, CMatrix>>>::iterator itEntity = m_mEntityMatrices.begin(); itEntity != m_mEntityMatrices.end(); itEntity++) {
		vector<pair<IEntity*, CMatrix>>& vMatrices = itEntity->second;
		for (int i = 0; i < vMatrices.size(); i++) {
			IEntity* pEntity = m_oEntityManager.CreateObject(string("Meshes/") + itEntity->first);
			if (pEntity) {
				pEntity->Link(m_pScene);
				vMatrices[i].first = pEntity;
				pEntity->SetLocalMatrix(vMatrices[i].second);
				pEntity->SetWeight(1);
				m_vEntities.push_back(pEntity);
			}
		}
	}
	if (m_bEditionMode)
		InitCamera();
}

void CWorldEditor::HandleSceneLoadingComplete(void* pWorldEditorData)
{
	CWorldEditor* pWorldEditor = (CWorldEditor*)pWorldEditorData;
	try {
		if (pWorldEditor) {
			pWorldEditor->OnSceneLoaded();
		}
	}
	catch (CEException& e)
	{
		pWorldEditor->m_oConsole.Println(e.what());
	}
	pWorldEditor->m_pScene->UnhandleLoadingComplete();
	pWorldEditor->m_pScene->OnChangeSector();
}