#include "IRenderer.h"

void IRenderer::SetDebugString( std::string s )
{
	m_sDebugString = s;
}

void IRenderer::GetDebugString( std::string& s ) const
{
	s = m_sDebugString;
}

void IRenderer::DisplayDebugInfos( bool bDisplay )
{
	m_bDisplayDebugInfos = bDisplay;
}

bool IRenderer::IsDisplayDebugInfos() const
{
	return m_bDisplayDebugInfos;
}
//
//void IRenderer::SetCurrentObjectWorldTM( CMatrix& m )
//{
//	m_mCurrentObjectWorldTM = m;
//}

void IRenderer::SetCurrentObjectLocalTM( CMatrix& m )
{
	m_mCurrentObjectLocalTM = m;
}

void IRenderer::SetCurrentCameraMatrix( CMatrix& m )
{
	m_oCameraMatrix = m;
}