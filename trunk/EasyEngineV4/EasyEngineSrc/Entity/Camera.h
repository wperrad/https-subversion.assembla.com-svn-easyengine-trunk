#ifndef CAMERA_H
#define CAMERA_H

#include "ICamera.h"
#include "../Utils2/Node.h"


class CCamera : public ICamera
{
public:
	CCamera( float fFov, IRenderer& oRenderer);
	virtual ~CCamera();
	void				Freeze( bool bFreeze);	
	float				GetFov();
	IGeometry*			GetBoundingGeometry() { return NULL; }
	void				Update();
	void				DrawBoundingBox(bool bDraw) {}
	void				SetShader(IShader* pShader) {}
	IBox*				GetBBox() { return NULL; }
	IRessource*			GetRessource() { return NULL; }
	void				SetWeight(float fWeight) {}
	float				GetWeight() { return 0.f; }
	void				SetRessource(string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, bool bDuplicate = false) {}
	void				AddAnimation(std::string sAnimationFile) {}
	void				SetCurrentAnimation(std::string sAnimation) {}
	IAnimation*			GetCurrentAnimation() { return NULL; }
	bool				HasAnimation(string sAnimationName) { return false; }
	void				DetachCurrentAnimation() {}
	IBone*				GetSkeletonRoot() { return NULL; }
	void				Hide(bool bHide) {}
	void				SetScaleFactor(float x, float y, float z) {}
	void				RunAction(string sAction, bool bLoop) {}
	void				LinkEntityToBone(IEntity* pChild, IBone* pParentBone, TLinkType = ePreserveChildRelativeTM) {}
	void				SetAnimationSpeed(TAnimation eAnimationType, float fSpeed) {}
	TAnimation			GetCurrentAnimationType() const { return eNone; }
	void				GetTypeName(string& sName) {}
	void				SetRenderingType(IRenderer::TRenderType t) {}
	void				DrawBoundingSphere(bool bDraw) {}
	void				DrawBoneBoundingSphere(int nID, bool bDraw) {}
	void				DrawAnimationBoundingBox(bool bDraw) {}
	float				GetBoundingSphereRadius() const { return 0.f; }
	void				Goto(const CVector& oPosition, float fSpeed) { throw 1; }
	void				DisplayViewCone(bool display);

protected:
	
	bool				m_bFreeze;
	float				m_fFov;
	bool				m_bDisplayViewCone;
	IRenderer&			m_oRenderer;
};

#endif // CAMERA_H