#include "glColorMap.h"

static unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

glColorMap::glColorMap(int _width, int _height, int _colorBuffers):
	glFrameBuffer(_width, _height),
	m_colorBuffers(_colorBuffers)
{
	bind();

	colors = new glTexture *[m_colorBuffers];

	// need to read back in next stage => use a TextureBuffer
	// 0 -> color buffer
	colors[0] = new glTexture(m_size.x, m_size.y, GL_RGBA);
	bindTexture(colors[0], GL_COLOR_ATTACHMENT0);
	// 1 -> bloom bluffer
	colors[1] = new glTexture(m_size.x, m_size.y, GL_RGBA);
	bindTexture(colors[1], GL_COLOR_ATTACHMENT1);

	glDrawBuffers(m_colorBuffers, attachments);

	// don't need to read the content of the depth & stencil buffer later => use a RenderBuffer
	depth_stencil = new glRenderBuffer(m_size.x, m_size.y);

	bindDepth(depth_stencil);
	unbind();
}

void glColorMap::clear(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

glRenderBuffer* glColorMap::get_stencil(void)
{
	return depth_stencil;
}

glTexture *glColorMap::getColorTexture(int index)
{
	if (index >= 0 && index < m_colorBuffers)
		return colors[index];

	return nullptr;
}

void glColorMap::bindColors(int colorBuffers)
{
	if (m_colorBuffers == 1)
		return;

	if (colorBuffers == -1) {
		// restore previous value
		glDrawBuffers(m_prev_colorBuffers, attachments);
	}
	else {
		m_prev_colorBuffers = m_colorBuffers;
		glDrawBuffers(colorBuffers, attachments);
	}
}

glColorMap::~glColorMap()
{
	for (auto i = 0; i < m_colorBuffers; i++) {
		delete colors[i];
	}
	delete colors;

	delete depth_stencil;
}