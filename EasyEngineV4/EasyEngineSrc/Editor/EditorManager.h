#pragma once

#include "IEditor.h"

class CEditor;

class CEditorManager : public IEditorManager
{
public:
	CEditorManager(EEInterface& oInterface);
	string		GetName() override;
	IEditor*	GetEditor(IEditor::Type type) override;
	void		CloseAllEditorButThis(CEditor* pEditor);

private:

	map<IEditor::Type, CEditor*>	m_mEditors;
};

extern "C" _declspec(dllexport) IEditorManager* CreateEditorManager(EEInterface& oInterface);