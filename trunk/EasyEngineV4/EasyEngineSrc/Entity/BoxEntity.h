#include "Shape.h"

class IBox;

class CBoxEntity : public CShape
{
	IBox&	m_oBox;

public:
	CBoxEntity( IRenderer& oRenderer, IBox& oBox );
	void	Update();
	IBox&	GetBox();
};