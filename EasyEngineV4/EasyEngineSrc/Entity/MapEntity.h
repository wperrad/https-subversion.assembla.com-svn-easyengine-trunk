#pragma once
#include "Entity.h"

class ICamera;

class CMinimapEntity : public CEntity
{
public:
	CMinimapEntity(EEInterface& oInterface, const string& sFileName, bool bDuplicate = false);

	~CMinimapEntity();

	void				Update();
	void                Link(CNode* pNode);

private:
	ICameraManager&		m_oCameraManager;
	int					m_nMapWidth;
	int					m_nMapHeight;
	const string		m_sFirstPassShaderName;
	const string		m_sSecondPassShaderName;
};

