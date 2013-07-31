#include "DebugTool.h"
#include <sstream>
#include <iomanip>

using namespace std;

CDebugTool::CDebugTool():
m_nNumberPrecision( 5 ),
m_fCap( 0 ),
m_nWidth( 0 )
{
}

void CDebugTool::SetNumberPrecision( int nPrecision )
{
	m_nNumberPrecision = nPrecision;
}

void CDebugTool::SetNumberWidth( int nWidth )
{
	m_nWidth = nWidth;
}

void CDebugTool::SerializeMatrix( const CMatrix& m, float fMin, string& sOut )
{
	ostringstream oss;
	oss << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_00, fMin ) << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_01, fMin ) << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_02, fMin ) << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_03, fMin ) << "\n";
	oss << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_10, fMin ) << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_11, fMin ) << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_12, fMin ) << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_13, fMin ) << "\n";
	oss << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_20, fMin ) << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_21, fMin ) << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_22, fMin ) << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_23, fMin ) << "\n";
	oss << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_30, fMin ) << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_31, fMin ) << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_32, fMin ) << setw(m_nWidth) << setprecision(m_nNumberPrecision) << GetCaped( m.m_33, fMin ) << "\n";
	sOut = oss.str();
}

float CDebugTool::GetCaped( float fNumber, float cap )
{
	if( fNumber > cap || fNumber < -cap )
		return fNumber;
	return 0.f;
}

void CDebugTool::SerializeVector( const CVector& v, string& sOut )
{
	ostringstream oss;
	oss << "( " << setprecision(m_nNumberPrecision) << v.m_x << ", " << setprecision(m_nNumberPrecision) << v.m_y << " , " << setprecision(m_nNumberPrecision) << v.m_z << " )";
	sOut = oss.str();
}