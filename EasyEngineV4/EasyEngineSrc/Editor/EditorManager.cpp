#include "EditorManager.h"
#include "MapEditor.h"

CEditorManager::CEditorManager(EEInterface& oInterface) :
IEditorManager(oInterface)
{
	m_mEditors[IEditor::Type::eMap] = new CMapEditor(oInterface);
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


extern "C" _declspec(dllexport) IEditorManager* CreateEditorManager(EEInterface& oInterface)
{
	return new CEditorManager(oInterface);
}