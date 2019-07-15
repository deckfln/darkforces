#include "glGBuffer.h"

static unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

glGBuffer::glGBuffer(int _width, int _height):
	glFrameBuffer(_width, _height),
{
	bind();
	m_colorBuffers = 3

	// need to read back in next stage => use a TextureBuffer
	color = new glTexture(m_size.x, m_size.y, GL_RGBA);
	bindTexture(color, GL_COLOR_ATTACHMENT0);

	normal = new glTexture(m_size.x, m_size.y, GL_RGB16F, 3, GL_NEAREST);
	bindTexture(normal, GL_COLOR_ATTACHMENT1);

	world = new glTexture(m_size.x, m_size.y, GL_RGB16F, 3, GL_NEAREST);
	bindTexture(world, GL_COLOR_ATTACHMENT2);

	glDrawBuffers(3, attachments);

	// don't need to read the content of the depth & stencil buffer later => use a RenderBuffer
	depth_stencil = new glRenderBuffer(m_size.x, m_size.y);

	bindDepth(depth_stencil);
	unbind();
}

glGBuffer::~glGBuffer()
{
	delete depth_stencil;
	delete world;
	delete normal;
	delete color;
}