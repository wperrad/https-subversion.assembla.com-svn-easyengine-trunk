#include "Shape.h"


class CRepere : public CShape
{
	CMatrix		m_oBase;
public:
	CRepere( IRenderer& oRenderer );
	void			Update();
	void			Colorize(float r, float g, float b, float a) {}
};