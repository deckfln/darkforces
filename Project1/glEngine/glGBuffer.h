#pragma once

#include "glColorMap.h"
#include "glTexture.h"
#include "glRenderBuffer.h"

enum { GBUFFER_COLOR, GBUFFER_NORMAL, GBUFFER_WORLD, GBUFFER_MATERIAL };

class glGBuffer : public glColorMap
{
public:
	glGBuffer(int _width, int _height);
	~glGBuffer();
};;