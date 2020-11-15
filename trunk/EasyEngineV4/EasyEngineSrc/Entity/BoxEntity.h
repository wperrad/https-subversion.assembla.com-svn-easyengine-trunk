#include "Shape.h"

class IBox;

class CBoxEntity : public CShape
{
	IBox&	m_oBox;

public:
	CBoxEntity( IRenderer& oRenderer, IBox& oBox );
	void	Update();
	IBox&	GetBox();
	void	Colorize(float r, float g, float b, float a) {}
};