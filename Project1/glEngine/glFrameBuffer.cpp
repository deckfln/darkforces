#include "glFrameBuffer.h"

glFrameBuffer::glFrameBuffer()
{
	glGenFramebuffers(1, &id);
}

void glFrameBuffer::bind(GLuint target)
{
	latest_bind = target;
	glBindFramebuffer(target, id);
}

void glFrameBuffer::bindTexture(glTexture *texture, GLuint attachment)
{
	glFramebufferTexture2D(latest_bind, attachment, GL_TEXTURE_2D, texture->getID(), 0);
}

void glFrameBuffer::bindDepth(glRenderBuffer *depth_stencil)
{
	glFramebufferRenderbuffer(latest_bind, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil->getID());
}

void glFrameBuffer::unbind(void)
{
	glBindFramebuffer(latest_bind, 0);
}

glFrameBuffer::~glFrameBuffer()
{
	glDeleteFramebuffers(1, &id);
}