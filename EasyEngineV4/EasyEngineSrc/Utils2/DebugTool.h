#include "math/Matrix.h"
#include "math/Vector.h"


#include <string>

class CDebugTool
{
	int		m_nNumberPrecision;
	int		m_nWidth;
	float	m_fCap;
	float	GetCaped( float fNumber, float cap );
public:
			CDebugTool();
	void	SetNumberPrecision( int nPrecision );
	void	SetNumberWidth( int nWidth );
	//void	SetNumberCap( float fCap );
	void	SerializeMatrix( const CMatrix& m, float fMin, std::string& sOut );
	void	SerializeVector( const CVector& v, std::string& sOut );
};