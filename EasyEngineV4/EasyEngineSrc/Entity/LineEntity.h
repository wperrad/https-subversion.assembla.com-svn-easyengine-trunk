#include "Shape.h"

class CLineEntity : public CShape
{
	CVector		m_oFirst;
	CVector		m_oLast;

public:

	CLineEntity( IRenderer& oRenderer );
	CLineEntity( IRenderer& oRenderer, const CVector& first, const CVector& last );
	void	Set( const CVector& first, const CVector& last );
	void	Update();
};