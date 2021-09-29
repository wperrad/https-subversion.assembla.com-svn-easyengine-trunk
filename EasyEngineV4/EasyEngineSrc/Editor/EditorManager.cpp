#include "EditorManager.h"
#include "MapEditor.h"
#include "WorldEditor.h"
#include "CharacterEditor.h"

CEditorManager::CEditorManager(EEInterface& oInterface) :
IEditorManager(oInterface)
{
	m_mEditors[IEditor::Type::eMap]			= new CMapEditor(oInterface, ICameraManager::TFree);
	m_mEditors[IEditor::Type::eWorld]		= new CWorldEditor(oInterface, ICameraManager::TFree);
	m_mEditors[IEditor::Type::eCharacter]	= new CCharacterEditor(oInterface, ICameraManager::TEditor);
}

string	CEditorManager::GetName()
{
	return "EditorManager";
}

IEditor* CEditorManager::GetEditor(IEditor::Type type)
{
	map<IEditor::Type, CEditor*>::iterator itEditor = m_mEditors.find(type);
	if (itEditor != m_mEditors.end()) {
		return static_cast<IEditor*>(itEditor->second);
	}
	return nullptr;
}

void CEditorManager::CloseAllEditorButThis(CEditor* pEditor)
{
	for (map<IEditor::Type, CEditor*>::iterator it = m_mEditors.begin(); it != m_mEditors.end(); it++)
		if(it->second != pEditor)
			it->second->SetEditionMode(false);
}

extern "C" _declspec(dllexport) IEditorManager* CreateEditorManager(EEInterface& oInterface)
{
	return new CEditorManager(oInterface);
}