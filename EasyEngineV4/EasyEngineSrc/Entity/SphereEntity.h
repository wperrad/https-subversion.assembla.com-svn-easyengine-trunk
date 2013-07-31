#ifndef SPHEREENTITY_H
#define SPHEREENTITY_H

#include "Shape.h"
#include "IGeometry.h"

class CSphereEntity : public CShape
{
	ISphere&		m_oSphere;
public:
	CSphereEntity( IRenderer& oRenderer, ISphere& oSphere );
	void Update();
	ISphere&		GetSphere();
};

#endif // SPHEREENTITY_H