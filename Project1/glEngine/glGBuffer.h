#pragma once

#include "glFrameBuffer.h"
#include "glTexture.h"
#include "glRenderBuffer.h"

class glGBuffer : public glFrameBuffer
{
	glTexture *color = nullptr;
	glTexture *world = nullptr;
	glTexture *normal = nullptr;
	glTexture* material = nullptr;
	glRenderBuffer *depth_stencil = nullptr;

public:
	glGBuffer();
	glGBuffer(int _width, int _height);
	glRenderBuffer* get_stencil(void);
	void clear(void);
	glTexture* getColorTexture(int nb=0);
	~glGBuffer();
};;