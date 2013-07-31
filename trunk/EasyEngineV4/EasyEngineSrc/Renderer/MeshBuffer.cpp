#include "MeshBuffer.h"

CGeometryBuffer::CGeometryBuffer():
m_nIndexCount( 0 ),
m_nID( -1 ),
m_nUVIndexCount( 0 )
{
}

CGeometryBuffer::CGeometryBuffer(unsigned int nIndexCount, unsigned int nUVIndexCount, unsigned int nBufferID)
{
	m_nIndexCount = nIndexCount;
	m_nID = nBufferID;
	m_nUVIndexCount = nUVIndexCount;
}

CGeometryBuffer::~CGeometryBuffer(void)
{
}

unsigned int CGeometryBuffer::GetIndexCount() const
{
	return m_nIndexCount;
}

unsigned int CGeometryBuffer::GetID() const
{
	return m_nID;
}

unsigned int CGeometryBuffer::GetUVIndexCount() const
{
	return m_nUVIndexCount;
}

CIndexedGeometryBuffer::CIndexedGeometryBuffer() :
m_nVertexBufferSize(0),
m_nIndexBufferSize(0),
m_nUVVertexBufferSize(0),
m_nUVIndexBufferSize(0),
m_nIndexOffset(0),
m_nNormalOffset(0),
m_nTextureOffset(0),
m_nVertexBufferID(0),
m_nIndexBufferID(0),
m_nUVVertexBufferID(0),
m_nUVIndexBufferID(0),
m_nNormalBufferID( -1 ),
m_nAttributeOffset( 0 ),
m_nIndexCount( 0 )
{
}