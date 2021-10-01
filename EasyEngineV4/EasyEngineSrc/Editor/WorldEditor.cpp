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

}

float CWorldEditor::GetPlanHeight()
{
	if (m_pCurrentAddedEntity) {
		ICamera* pCamera = m_oCameraManager.GetActiveCamera();
		CVector camPos;
		pCamera->GetWorldPosition(camPos);
		IBox* pBox = dynamic_cast<IBox*>(m_pCurrentAddedEntity->GetBoundingGeometry());
		if (pBox)
			return camPos.m_y - pBox->GetDimension().m_y;
	}
	return 0.f;
}

void CWorldEditor::OnEntitySelected()
{

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
			m_pScene->HandleLoadingComplete(OnSceneLoadingComplete, this);
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
	}
}

void CWorldEditor::SpawnEntity(string sID)
{
	throw CMethodNotImplementedException("CWorldEditor::SpawnEntity()");
	if(!m_bEditionMode)
		SetEditionMode(true);

	m_pCurrentAddedEntity = m_oEntityManager.BuildCharacterFromDatabase(sID, m_pScene);
	m_oEntityManager.AddEntity(m_pCurrentAddedEntity, sID);
	InitSpawnedEntity();
	CMatrix m;
	m_mCharacterMatrices[sID] = m;
}

void CWorldEditor::RemoveCharacter(string sID)
{
	m_mCharacterMatrices.erase(sID);
	IEntity* pCharacter = m_oEntityManager.GetEntity(sID);
	vector<IEntity*>::iterator itCharacter = std::find(m_vCharacters.begin(), m_vCharacters.end(), pCharacter);
	m_vCharacters.erase(itCharacter);
}

void CWorldEditor::SpawnCharacter(string sID)
{
	if (!m_bEditionMode)
		SetEditionMode(true);

	m_pCurrentAddedEntity = m_oEntityManager.BuildCharacterFromDatabase(sID, m_pScene);
	if (!m_pCurrentAddedEntity) {
		ostringstream oss;
		oss << "Erreur : Personnage " << sID << " introuvable dans la base de donnee des personnages.";
		throw CEException(oss.str());
	}
	m_oEntityManager.AddEntity(m_pCurrentAddedEntity, sID);
	InitSpawnedEntity();
	CMatrix m;
	m_mCharacterMatrices[sID] = m;
	m_vCharacters.push_back(m_pCurrentAddedEntity);
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
	entities = m_vCharacters;
}

void CWorldEditor::Edit(string id)
{
	SetEditionMode(true);
	Load(id);
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
	m_vCharacters.clear();
	m_mCharacterMatrices.clear();
	m_oEntityManager.Clear();
}

void CWorldEditor::OnSceneLoadingComplete(void* pWorldEditorData)
{
	CWorldEditor* pWorldEditor = (CWorldEditor*)pWorldEditorData;
	try {
		if (pWorldEditor) {
			for (map<string, CMatrix>::iterator itCharacter = pWorldEditor->m_mCharacterMatrices.begin(); itCharacter != pWorldEditor->m_mCharacterMatrices.end(); itCharacter++) {
				IEntity* pEntity = nullptr;
				if (itCharacter != pWorldEditor->m_mCharacterMatrices.end()) {
					pEntity = pWorldEditor->m_oEntityManager.BuildCharacterFromDatabase(itCharacter->first, pWorldEditor->m_pScene);
					if (pEntity) {
						pEntity->SetLocalMatrix(itCharacter->second);
						pEntity->SetWeight(1);
						pWorldEditor->m_vCharacters.push_back(pEntity);
					}
				}
				if ((itCharacter == pWorldEditor->m_mCharacterMatrices.end()) || !pEntity) {
					ostringstream oss;
					oss << "Erreur dans CWorldEditor::OnSceneLoadingComplete() : le personnage " << itCharacter->first << " n'existe pas.";
					pWorldEditor->m_oConsole.Println(oss.str());
				}
			}
			if(pWorldEditor->m_bEditionMode)
				pWorldEditor->InitCamera();
		}
	}
	catch (CEException& e)
	{
		pWorldEditor->m_oConsole.Println(e.what());
	}
	pWorldEditor->m_pScene->UnhandleLoadingComplete();
	pWorldEditor->m_pScene->OnChangeSector();
}