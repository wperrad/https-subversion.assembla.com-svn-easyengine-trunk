#define TEXTUREBUFFER_CPP

#include "texturebuffer.h"

CTextureBuffer::CTextureBuffer():
m_nID( -1 )
{
}

CTextureBuffer::CTextureBuffer( unsigned int nTextureID )
{
	m_nID = nTextureID;
}

CTextureBuffer::~CTextureBuffer(void)
{
}

unsigned int CTextureBuffer::GetID() const
{
	return m_nID;
}