#include "Camera.h"

CCamera::CCamera(float fFov, IRenderer& oRenderer):
m_bFreeze( false ),
m_fFov( fFov ),
m_oRenderer(oRenderer),
m_bDisplayViewCone(false)
{
}


CCamera::~CCamera()
{

}

void CCamera::Freeze( bool bFreeze )
{
	m_bFreeze = bFreeze;
}

float CCamera::GetFov()
{
	return m_fFov;
}

void CCamera::DisplayViewCone(bool display)
{
	m_bDisplayViewCone = display;
}

void CCamera::Update()
{
	CNode::Update();
	if (m_bDisplayViewCone) {
		m_oRenderer.CullFace(0);
		CMatrix backupModelMatrix;
		m_oRenderer.GetModelMatrix(backupModelMatrix);
		m_oRenderer.SetModelMatrix(m_oWorldMatrix);
		m_oRenderer.DrawCylinder(50, 1, 100, 10, 10);
		m_oRenderer.CullFace(1);
		m_oRenderer.SetModelMatrix(backupModelMatrix);
	}
}