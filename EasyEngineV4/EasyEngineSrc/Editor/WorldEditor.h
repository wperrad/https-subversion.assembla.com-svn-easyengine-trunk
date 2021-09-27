#pragma once

#include "IEditor.h"
#include "SpawnableEditor.h"
#include "../Utils/Math/Matrix.h"

class IFileSystem;
class ISceneManager;

class CWorldEditor : public IWorldEditor, public CSpawnableEditor
{
public:
	CWorldEditor(EEInterface& oInterface);
	void						HandleMapLoaded(string sMapName);
	void						ClearWorld();
	void						Load(string fileName) override;

private:
	void						OnEntityAdded() override;
	float						GetPlanHeight() override;
	void						OnEntitySelected() override;
	string						GetName() override;
	void						Save(string sFileName) override;
	void						SpawnEntity(string id);
	void						SetEditionMode(bool bEditionMode) override;
	void						GetRelativeDatabasePath(string& path);
	static void					OnSceneLoadingComplete(void* pWorldEditor);

	IFileSystem&				m_oFileSystem;
	ISceneManager&				m_oSceneManager;
	map<string, CVector>		m_mMaps;
	map<string, CMatrix>		m_mCharacters;
	const string				m_sDatabaseFileName = "world.db";
};