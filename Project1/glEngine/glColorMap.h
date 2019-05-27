#pragma once

#include "../glEngine/glFrameBuffer.h"
#include "../glEngine/glRenderBuffer.h"
#include "../glEngine/glTexture.h"

class glColorMap: public glFrameBuffer
{
	glTexture *color;
	glRenderBuffer *depth_stencil;

public:
	glColorMap(int width, int height);
	void clear(void);
	glTexture *getColorTexture(void);
	~glColorMap();
};