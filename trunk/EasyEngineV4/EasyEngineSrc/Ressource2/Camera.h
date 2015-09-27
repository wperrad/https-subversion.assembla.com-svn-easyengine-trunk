#ifndef CAMERA_H
#define CAMERA_H

#pragma warning(disable:4251)

// stl
#include <string>

// Engine
#include "Ressource2API.h"
#include "math/matrix.h"

class CCamera
{
	CMatrix							m_matRotation;
	CMatrix							m_matTranslation;
	float							m_fSpeed;
	bool							m_bFreeze;
	std::string						m_sName;
public:
									CCamera(void);
  virtual							~CCamera(void);

  void                				Translate(float dx , float dy , float dz);	/*
  void								Yaw(float fAngle);
  void								Pitch(float fAngle);
  void								Roll(float fAngle);*/


  void								SetGlobalRotationMatrix(CMatrix mRot);
  void								SetGlobalTranslationMatrix(CMatrix mRot);

  //void								SetXFormTM(CMatrix mat);
  CMatrix							GetXFormTM();
  void								SetWorldPosition(float x, float y, float z);  
  CVector							GetWorldPosition();

  void								Update(float Yaw, float Pitch, float fadvance, float fLeft);
  void								SetSpeed(float fSpeed);
  float								GetSpeed();
  void								Freeze(bool bFreeze);

  void								SetName( const std::string& sName );
  void								GetName( std::string& sName );

};


#endif