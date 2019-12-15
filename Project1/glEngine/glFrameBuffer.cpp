#include "glFrameBuffer.h"

glFrameBuffer::glFrameBuffer()
{

}

glFrameBuffer::glFrameBuffer(int _width, int _height):
	m_size(_width, _height)
{
	glGenFramebuffers(1, &id);
}

void glFrameBuffer::bind()
{
	// push the current framebuffer
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_prevFBO);
	glGetIntegerv(GL_VIEWPORT, m_prevViewport);

	glViewport(0, 0, m_size.x, m_size.y);
	glBindFramebuffer(type, id);
}

void glFrameBuffer::bindTexture(glTexture *texture, GLuint attachment)
{
	//todo: let openGL find out what is the type of the texture (1D, 2D, 3D, cubmap ...)
	glFramebufferTexture(type, attachment, texture->getID(), 0);
}

void glFrameBuffer::bindDepth(glRenderBuffer *depth_stencil)
{
	if (depth_stencil != nullptr) {
		glFramebufferRenderbuffer(type, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil->getID());
	}
	else {
		glFramebufferRenderbuffer(type, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
	}
}

void glFrameBuffer::unbind(void)
{
	glBindFramebuffer(type, m_prevFBO);
	glViewport(m_prevViewport[0], m_prevViewport[1], m_prevViewport[2], m_prevViewport[3]);
}

void glFrameBuffer::resize(int _width, int _height)
{
	m_size.x = _height;
	m_size.y = _width;
}

glm::ivec2 &glFrameBuffer::size(void)
{
	return m_size;
}

void glFrameBuffer::copyFrom(glFrameBuffer* source, int mask)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, source->id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
	glBlitFramebuffer(0, 0, m_size.x, m_size.y, 0, 0, m_size.x, m_size.y, mask, GL_NEAREST);
}

glFrameBuffer::~glFrameBuffer()
{
	glDeleteFramebuffers(1, &id);
}