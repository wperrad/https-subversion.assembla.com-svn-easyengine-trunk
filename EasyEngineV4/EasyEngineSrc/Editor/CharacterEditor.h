#pragma once

#include "IEditor.h"
#include "Editor.h"
#include "IEventDispatcher.h"

class EEInterface;
class IEntityManager;
class ISceneManager;
class IInputManager;
class ICameraManager;
class IScene;
class ICharacter;
class CPlugin;
class CWorldEditor;
class ICamera;

class CCharacterEditor : public ICharacterEditor, public CEditor
{
public:
	CCharacterEditor(EEInterface& oInterface, ICameraManager::TCameraType type);
	void					SetEditionMode(bool bEditionMode) override;
	void					Load(string sCharacterId) override;
	void					Save() override;
	void					HandleEditorManagerCreation(IEditorManager* pEditor) override;
	string					GetName() override;
	bool					IsEnabled() override;
	void					SpawnEntity(string sEntityFileName) override;
	void					SetCurrentEditablePlayer(ICharacter* pPlayer) override;
	void					SetCurrentEditableNPC(ICharacter* pNPCEntity) override;
	void					AddHairs(string sHairsName) override;
	void					WearShoes(string sShoesName) override;
	void					SetTexture(string sTexture) override;
	void					Edit(string id) override;

private:

	void					InitSpawnedCharacter();
	static void				OnMouseEventCallback(CPlugin* plugin, IEventDispatcher::TMouseEvent e, int x, int y);
	static void				HandleEditorCreation(CPlugin* pPlugin, void* pDatar);
	IScene*					m_pScene;
	ICharacter*				m_pCurrentCharacter;
	bool					m_bIsLeftMousePressed;
	int						m_nMousePosX;
	CWorldEditor*			m_pWorldEditor;
};