#pragma once

#include "IEditor.h"

class CEditorManager : public IEditorManager
{
public:
	CEditorManager(EEInterface& oInterface);
	string		GetName() override;
	IEditor*	GetEditor(IEditor::Type type) override;
	void		CloseAllEditor() override;

private:

	map<IEditor::Type, IEditor*>	m_mEditors;
};

extern "C" _declspec(dllexport) IEditorManager* CreateEditorManager(EEInterface& oInterface);