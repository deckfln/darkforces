#pragma once

#include "glColormap.h"
#include "glTexture.h"
#include "glRenderBuffer.h"

class glGBuffer : public glColorMap
{
	glTexture *color = nullptr;
	glTexture *world = nullptr;
	glTexture *normal = nullptr;
	glRenderBuffer *depth_stencil = nullptr;

public:
	glGBuffer(int _width, int _height);
	~glGBuffer();
};