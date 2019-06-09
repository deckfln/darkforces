#include "glFrameBuffer.h"

glFrameBuffer::glFrameBuffer(int _width, int _height):
	height(_height),
	width(_width)
{
	glGenFramebuffers(1, &id);
}

void glFrameBuffer::bind()
{
	// push the current framebuffer
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_prevFBO);
	glGetIntegerv(GL_VIEWPORT, m_prevViewport);

	glViewport(0, 0, width, height);
	glBindFramebuffer(type, id);
}

void glFrameBuffer::bindTexture(glTexture *texture, GLuint attachment)
{
	glFramebufferTexture2D(type, attachment, GL_TEXTURE_2D, texture->getID(), 0);
}

void glFrameBuffer::bindDepth(glRenderBuffer *depth_stencil)
{
	glFramebufferRenderbuffer(type, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil->getID());
}

void glFrameBuffer::unbind(void)
{
	glBindFramebuffer(type, m_prevFBO);
	glViewport(m_prevViewport[0], m_prevViewport[1], m_prevViewport[2], m_prevViewport[3]);
}

void glFrameBuffer::resize(int _width, int _height)
{
	height = _height;
	width = _width;
}

glFrameBuffer::~glFrameBuffer()
{
	glDeleteFramebuffers(1, &id);
}