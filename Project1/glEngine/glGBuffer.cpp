#include "glGBuffer.h"

static unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

glGBuffer::glGBuffer()
{
}

glGBuffer::glGBuffer(int _width, int _height):
	glFrameBuffer(_width, _height)
{
	bind();

	color = new glTexture(m_size.x, m_size.y, GL_RGB);
	bindTexture(color, GL_COLOR_ATTACHMENT0);

	normal = new glTexture(m_size.x, m_size.y, GL_RGB16F, 3, GL_NEAREST);
	bindTexture(normal, GL_COLOR_ATTACHMENT1);

	world = new glTexture(m_size.x, m_size.y, GL_RGB16F, 3, GL_NEAREST);
	bindTexture(world, GL_COLOR_ATTACHMENT2);

	material = new glTexture(m_size.x, m_size.y, GL_RGB, 3, GL_NEAREST);
	bindTexture(material, GL_COLOR_ATTACHMENT3);

	glDrawBuffers(4, attachments);

	depth_stencil = new glRenderBuffer(m_size.x, m_size.y);

	bindDepth(depth_stencil);
	unbind();
}

glRenderBuffer* glGBuffer::get_stencil(void)
{
	return depth_stencil;
}

glTexture* glGBuffer::getColorTexture(int nb)
{
	switch(nb) {
	case 0: return color; break;
	case 1: return normal; break;
	case 2: return world; break;
	case 3: return material; break;
	}
}

void glGBuffer::clear(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

glGBuffer::~glGBuffer()
{
	delete depth_stencil;
	delete world;
	delete normal;
	delete color;
	delete material;
}