#include "glGBuffer.h"

static unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

glGBuffer::glGBuffer(int _width, int _height):
	glColorMap(_width, _height)
{
	m_size.x = _width;
	m_size.y = _height;
	m_totalBuffers = m_allocatedBuffers = 4;
	m_mask = GL_COLORMAP_DEPTH | GL_COLORMAP_STENCIL;

	bind();

	colors = new glTexture * [m_totalBuffers];

	colors[GBUFFER_COLOR] = new glTexture(m_size.x, m_size.y, GL_RGBA);
	bindTexture(colors[GBUFFER_COLOR], GL_COLOR_ATTACHMENT0);

	colors[GBUFFER_NORMAL] = new glTexture(m_size.x, m_size.y, GL_RGB16F, 3, GL_NEAREST);
	bindTexture(colors[GBUFFER_NORMAL], GL_COLOR_ATTACHMENT1);

	colors[GBUFFER_WORLD] = new glTexture(m_size.x, m_size.y, GL_RGB16F, 3, GL_NEAREST);
	bindTexture(colors[GBUFFER_WORLD], GL_COLOR_ATTACHMENT2);

	colors[GBUFFER_MATERIAL] = new glTexture(m_size.x, m_size.y, GL_RGB, 3, GL_NEAREST);
	bindTexture(colors[GBUFFER_MATERIAL], GL_COLOR_ATTACHMENT3);

	glDrawBuffers(4, attachments);

	depth_stencil = new glRenderBuffer(m_size.x, m_size.y);

	bindDepth(depth_stencil);
	unbind();
}

glGBuffer::~glGBuffer()
{
}