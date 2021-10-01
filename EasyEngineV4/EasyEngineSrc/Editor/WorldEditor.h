#pragma once

#include "IEditor.h"
#include "SpawnableEditor.h"
#include "../Utils/Math/Matrix.h"

class IFileSystem;
class ISceneManager;

class CWorldEditor : public IWorldEditor, public CSpawnableEditor
{
public:
	CWorldEditor(EEInterface& oInterface, ICameraManager::TCameraType cameraType);
	void						HandleMapLoaded(string sMapName);
	void						ClearWorld();
	void						Load(string fileName) override;
	void						Edit(string id) override;

private:
	void						OnEntityAdded() override;
	float						GetPlanHeight() override;
	void						OnEntitySelected() override;
	string						GetName() override;
	void						Save(string sFileName) override;
	void						SpawnEntity(string id) override;
	void						RemoveCharacter(string sID) override;
	void						SpawnCharacter(string sID) override;
	void						SetEditionMode(bool bEditionMode) override;
	void						CollectSelectableEntity(vector<IEntity*>& entities) override;
	void						GetRelativeDatabasePath(string& path);
	void						ClearEntities();

	static void					OnSceneLoadingComplete(void* pWorldEditor);

	IFileSystem&				m_oFileSystem;
	ISceneManager&				m_oSceneManager;
	map<string, CVector>		m_mMaps;
	map<string, CMatrix>		m_mCharacterMatrices;
	vector<IEntity*>			m_vCharacters;
	const string				m_sDatabaseFileName = "world.db";
};