#ifndef ICAMERA_H
#define ICAMERA_H

#define ENTITYCAMERA

#include "../Utils2/Node.h"

#ifdef ENTITYCAMERA
#include "IEntity.h"
#endif // ENTITYCAMERA

#pragma warning(disable:4250)



class ICamera : 
#ifdef ENTITYCAMERA
	public IEntity
#else
	public CNode
#endif // ENTITYCAMERA
{

public:
	virtual float	GetSpeed() = 0;
	virtual void	SetSpeed( float fSpeed ) = 0;
	virtual void	Freeze( bool bFreeze ) = 0;
	virtual void	Move( float fOffsetYaw, float fOffsetPitch, float fOffsetRoll, float fAvanceOffet, float fLeftOffset, float fUpOffset ) = 0;
	virtual float	GetFov() = 0;
	virtual void	Zoom(int value) = 0;

#ifdef ENTITYCAMERA
	/*
	void				Update(){ CNode::Update(); }
	void				DrawBoundingBox( bool bDraw ){}
	void				SetShader( IShader* pShader ){}
	IBox*				GetBBox(){ return NULL; }
	IRessource*			GetRessource(){ return NULL; }
	void				SetWeight( float fWeight ){}
	float				GetWeight(){ return 0.f; }
	void				SetRessource( string sFileName, IRessourceManager& oRessourceManager, IRenderer& oRenderer, bool bDuplicate = false ){}
	void				AddAnimation( std::string sAnimationFile ){}
	void				SetCurrentAnimation( std::string sAnimation ){}
	IAnimation*			GetCurrentAnimation(){ return NULL; }
	bool				HasAnimation( string sAnimationName ){ return false; }
	void				DetachCurrentAnimation(){}
	IBone*				GetSkeletonRoot(){ return NULL; }
	void				Hide( bool bHide ){}
	void				SetScaleFactor( float x, float y, float z ){}
	void				RunAction( string sAction, bool bLoop ){}
	void				LinkEntityToBone( IEntity* pChild, IBone* pParentBone, TLinkType = ePreserveChildRelativeTM  ){}
	void				SetAnimationSpeed( TAnimation eAnimationType, float fSpeed ){}
	TAnimation			GetCurrentAnimationType() const{ return eNone; }
	void				GetTypeName( string& sName ){}
	void				SetRenderingType( IRenderer::TRenderType t ){}
	void				DrawBoundingSphere( bool bDraw ){}
	void				DrawBoneBoundingSphere( int nID, bool bDraw ){}
	void				DrawAnimationBoundingBox( bool bDraw ){}
	float				GetBoundingSphereRadius() const{ return 0.f; }
	void				Goto( const CVector& oPosition, float fSpeed ){ throw 1; }	*/

#endif // ENTITYCAMERA
	
};

#endif // ICAMERA_H