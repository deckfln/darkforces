#pragma once

#include <list>
#include <map>

#include "../glEngine/glFrameBuffer.h"
#include "../glEngine/glRenderBuffer.h"
#include "../glEngine/glTexture.h"

const int GL_COLORMAP_DEPTH = 1;
const int GL_COLORMAP_STENCIL = 2;

class glColorMap: public glFrameBuffer
{
protected:
	glTexture **colors;
	int m_allocatedBuffers = 0;
	int m_totalBuffers = 0;
	glRenderBuffer *depth_stencil;
	int m_prev_colorBuffers = 0;
	int m_mask = 0;		// all buffers on the map : DEPTH, STENCITL, COLOR
	std::map<int, GLboolean> m_previous_state;

public:
	glColorMap(int width, int height);
	glColorMap(int width, int height, int colorBuffers, int mask = 3, glTexture *extra=nullptr);
	void clear(void);
	void enable(int mask);
	void disable(int mask);
	void restore(int mask);
	glTexture *getColorTexture(int index);
	void bindColors(int colorBuffers);
	glRenderBuffer* get_stencil(void);
	~glColorMap();
};