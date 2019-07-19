#pragma once

#include <list>

#include "../glEngine/glFrameBuffer.h"
#include "../glEngine/glRenderBuffer.h"
#include "../glEngine/glTexture.h"

class glColorMap: public glFrameBuffer
{
	glTexture **colors;
	int m_colorBuffers = 0;
	glRenderBuffer *depth_stencil;
	int m_prev_colorBuffers = 0;

public:
	glColorMap(int width, int height, int colorBuffers=1);
	void clear(void);
	glTexture *getColorTexture(int index);
	void bindColors(int colorBuffers);
	glRenderBuffer* get_stencil(void);
	~glColorMap();
};