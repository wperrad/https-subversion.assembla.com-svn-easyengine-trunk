#ifndef CAMERA_TEST_H
#define CAMERA_TEST_H

#include "Camera.h"


class CLinkedCamera : public CCamera
{
public:
	CLinkedCamera( float fFov, IRenderer& oRenderer );
	virtual ~CLinkedCamera();
	void				Move( float fOffsetYaw, float fOffsetPitch, float fOffsetRoll, float fAvanceOffet, float fLeftOffset, float fUpOffset );
	void				Colorize(float r, float g, float b, float a) {}
	void				SetEntityName(string sName);
	void				GetEntityName(string& sName);
	void				Zoom(int value);
	void                Link(CNode* pNode);
	float				GetSpeed() { return 0.f; }
	void				SetSpeed(float fSpeed) {}
	void				Update();
	void				SetInventoryMode(bool bInventoryMode) {}

private:
	string				m_sEntityName;
	CNode*				m_pHeadNode;
	CNode*				m_pBehindNode;
};


#endif //CAMERA_TEST_H