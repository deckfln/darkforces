#pragma once
#include "glad/glad.h"
#include "glTexture.h"
#include "glRenderBuffer.h"

class glFrameBuffer
{
	GLuint id;
	GLuint texture;
	GLuint latest_bind;

public:
	glFrameBuffer();
	void bind(GLuint type = GL_FRAMEBUFFER);
	void bindTexture(glTexture *texture, GLuint attachment= GL_COLOR_ATTACHMENT0);
	void bindDepth(glRenderBuffer *depth_stencil);
	void unbind(void);
	~glFrameBuffer();
};