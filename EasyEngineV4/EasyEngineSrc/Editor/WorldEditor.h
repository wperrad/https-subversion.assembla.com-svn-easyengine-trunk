#pragma once

#include "IEditor.h"
#include "../Utils/Math/Matrix.h"

class CWorldEditor : public IWorldEditor
{
public:
	void Save(string sFileName) override;
	void AddCharacter(string sCharacterId, CMatrix& oWorldTM);

public:
	map<string, CMatrix>	m_mcharacters;
};