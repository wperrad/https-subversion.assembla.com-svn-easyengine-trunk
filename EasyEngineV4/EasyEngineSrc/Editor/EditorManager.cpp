#include "EditorManager.h"
#include "MapEditor.h"
#include "WorldEditor.h"

CEditorManager::CEditorManager(EEInterface& oInterface) :
IEditorManager(oInterface)
{
	m_mEditors[IEditor::Type::eMap]		= new CMapEditor(oInterface);
	m_mEditors[IEditor::Type::eWorld]	= new CWorldEditor(oInterface);
}

void CEditorManager::OnPluginRegistered()
{
	for (map<IEditor::Type, IEditor*>::iterator it = m_mEditors.begin(); it != m_mEditors.end(); it++) {
		it->second->HandleEditorManagerCreation(this);
	}
}

string	CEditorManager::GetName()
{
	return "EditorManager";
}

IEditor* CEditorManager::GetEditor(IEditor::Type type)
{
	map<IEditor::Type, IEditor*>::iterator itEditor = m_mEditors.find(type);
	if (itEditor != m_mEditors.end()) {
		return itEditor->second;
	}
	return nullptr;
}

void CEditorManager::CloseAllEditor()
{
	for (map<IEditor::Type, IEditor*>::iterator it = m_mEditors.begin(); it != m_mEditors.end(); it++)
		it->second->SetEditionMode(false);
}

extern "C" _declspec(dllexport) IEditorManager* CreateEditorManager(EEInterface& oInterface)
{
	return new CEditorManager(oInterface);
}