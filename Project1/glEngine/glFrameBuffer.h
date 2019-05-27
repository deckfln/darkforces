#pragma once
#include <stack>

#include "glad/glad.h"
#include "glTexture.h"
#include "glRenderBuffer.h"

class glFrameBuffer
{
	GLuint id;
	GLuint texture;
	GLuint latest_bind;
	GLint m_prevFBO;
	GLint m_prevViewport[4];

protected:
	int width;
	int height;
	int type = GL_FRAMEBUFFER;

public:
	glFrameBuffer(int width, int height);
	void bind();
	virtual void clear(void) {};
	void bindTexture(glTexture *texture, GLuint attachment= GL_COLOR_ATTACHMENT0);
	void bindDepth(glRenderBuffer *depth_stencil);
	void unbind(void);
	~glFrameBuffer();
};