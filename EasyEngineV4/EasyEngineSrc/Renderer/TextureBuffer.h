#ifndef TEXTUREBUFFER_CPP
#ifndef RENDERER_CPP
#error
#endif
#endif

#ifndef TEXTUREBUFFER_H
#define TEXTUREBUFFER_H

#include "IRenderer.h"


class CTextureBuffer : public IBuffer
{
	unsigned int m_nID;

public:
	CTextureBuffer();
	CTextureBuffer(unsigned int nTextureID);
	unsigned int	GetID() const;
	virtual ~CTextureBuffer();
};



#endif //TEXTUREBUFFER_H