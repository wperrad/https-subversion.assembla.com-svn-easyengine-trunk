#include "Shape.h"


class CRepere : public CShape
{
	CMatrix		m_oBase;
public:
	CRepere( IRenderer& oRenderer );
	void			Update();
};