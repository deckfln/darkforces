#include "glColorMap.h"

glColorMap::glColorMap(int _width, int _height):
	glFrameBuffer(_width, _height)
{
	bind();

	// need to read back in next stage => use a TextureBuffer
	color = new glTexture(m_size.x, m_size.y, GL_RGBA);
	bindTexture(color, GL_COLOR_ATTACHMENT0);

	// don't need to read the content of the depth & stencil buffer later => use a RenderBuffer
	depth_stencil = new glRenderBuffer(m_size.x, m_size.y);

	bindDepth(depth_stencil);
	unbind();
}

void glColorMap::clear(void)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

glTexture *glColorMap::getColorTexture(void)
{
	return color;
}

glColorMap::~glColorMap()
{
	delete color;
	delete depth_stencil;
}