#pragma once

#include "../glEngine/glFrameBuffer.h"
#include "../glEngine/glRenderBuffer.h"
#include "../glEngine/glTexture.h"

class FrameBuffer
{
	int width;
	int height;

	glFrameBuffer *frameBuffer;
	glTexture *color;
	glRenderBuffer *depth_stencil;

public:
	FrameBuffer(int width, int height);
	void bind(void);
	glTexture *get_colorBuffer(void);
	void unbind(void);
	~FrameBuffer();
};