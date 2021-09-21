#pragma once

#include "EEPlugin.h"

class IEditor : public CPlugin
{
public:
	enum Type
	{
		eMap = 0,
		eCharacer,
		eWorld
	};

	IEditor(EEInterface& oInterface) : CPlugin(nullptr, "") {}
	virtual			~IEditor() = 0 {}
	virtual void	SetEditionMode(bool bEditionMode) = 0;
	virtual void	Load(string fileName) = 0;
	virtual void	Save(string fileName) = 0;
	virtual void	DisplayPickingRay(bool enable) = 0;
};

class IMapEditor : public IEditor
{
public:
	IMapEditor(EEInterface& oInterface) : IEditor(oInterface) {}
	virtual void	SpawnEntity(string sEntityFileName) = 0;
	virtual void	SetGroundAdaptationHeight(float fHeight) = 0;
};

class ICharacterEditor : public IEditor
{
public:
	ICharacterEditor(EEInterface& oInterface) : IEditor(oInterface) {}
};

class IWorldEditor : public IEditor
{
public:
	IWorldEditor(EEInterface& oInterface) : IEditor(oInterface) {}
};


class IEditorManager : public CPlugin
{
public:
	IEditorManager(EEInterface& oInterface) : CPlugin(nullptr, "") {}
	virtual IEditor*	GetEditor(IEditor::Type type) = 0;
};