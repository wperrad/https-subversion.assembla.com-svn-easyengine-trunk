#include "Math/Vector.h"
#include "IGeometry.h"

class CCylinder : public ICylinder
{
	CVector		m_oBase;
	float		m_fRadius;
	float		m_fHeight;

public:

	CCylinder( const CVector& oBase, float fRadius, float fHeight );
	CVector&	GetBase();
	float		GetRadius();
	float		GetHeight();
	void		Set( const CVector& oBase, float fRadius, float fHeight );
	
};