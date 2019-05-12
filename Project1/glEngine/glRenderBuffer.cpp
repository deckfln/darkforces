#include "glRenderBuffer.h"

glRenderBuffer::glRenderBuffer(int width, int height)
{
	glGenRenderbuffers(1, &id);
	glBindRenderbuffer(GL_RENDERBUFFER, id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

GLuint glRenderBuffer::getID(void)
{
	return id;
}

glRenderBuffer::~glRenderBuffer()
{
	glDeleteRenderbuffers(1, &id);
}