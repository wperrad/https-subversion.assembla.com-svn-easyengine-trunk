#ifndef BUFFER_H
#define BUFFER_H

#include "IRenderer.h"

class CGeometryBuffer : public IBuffer
{
	unsigned int	m_nIndexCount;
	unsigned int	m_nID;
	unsigned int	m_nUVIndexCount;
	//unsigned int	m_nVertexAttributeCount;	
	
public:
	//unsigned int	m_nVertexWeightAttributeID;
	//unsigned int	m_nWeightedVertexIDAttributeID;
	//unsigned int	m_nIndexAttributeID;
	//unsigned int	m_nIndexAttributeSize;
	//int				m_nTextureID;

					CGeometryBuffer();
					CGeometryBuffer( unsigned int nIndexCount, unsigned int nUVIndexCount, unsigned int nBufferID );
	virtual			~CGeometryBuffer();
	unsigned int	GetIndexCount() const;
	unsigned int	GetUVIndexCount() const;
	unsigned int	GetVertexWeightAttributeCount() const;
	unsigned int	GetID() const;
};

class CIndexedGeometryBuffer : public IBuffer
{
public:
	unsigned int	m_nVertexBufferSize;
	unsigned int	m_nIndexBufferSize;
	unsigned int	m_nUVVertexBufferSize;
	unsigned int	m_nUVIndexBufferSize;
	//unsigned int	m_nAttributeBufferSize;

	unsigned int	m_nIndexOffset;
	unsigned int	m_nNormalOffset;
	unsigned int	m_nTextureOffset;
	unsigned int	m_nAttributeOffset;

	unsigned int	m_nIndexCount;

	unsigned int	m_nVertexBufferID;
	unsigned int	m_nIndexBufferID;
	unsigned int	m_nNormalBufferID;
	unsigned int	m_nUVVertexBufferID;
	unsigned int	m_nUVIndexBufferID;
	//unsigned int	m_nAttributeBufferID;
	//int				m_nTextureID;

	//int				m_nVertexWeightAttributeID;
	//int				m_nWeightedVertexIDAttributeID;

	CIndexedGeometryBuffer();
};

#endif //BUFFER_H