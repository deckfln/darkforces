#pragma once
#include <stack>
#include <glm/glm.hpp>
#include "../glad/glad.h"
#include "glTexture.h"
#include "glRenderBuffer.h"

class glFrameBuffer
{
	GLuint id = -1;
	GLuint texture = -1;
	GLuint latest_bind = -1;
	GLint m_prevFBO = -1;
	GLint m_prevViewport[4] = { -1, -1, -1,-1 };

protected:
	glm::vec2 m_size = glm::vec2(0);
	int type = GL_FRAMEBUFFER;

public:
	glFrameBuffer();
	glFrameBuffer(int width, int height);
	void bind();
	virtual void clear(void) {};
	void bindTexture(glTexture *texture, GLuint attachment= GL_COLOR_ATTACHMENT0);
	void bindDepth(glRenderBuffer *depth_stencil);
	void unbind(void);
	void resize(int _width, int _height);
	glm::vec2 &size(void);
	~glFrameBuffer();
};