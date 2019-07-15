#pragma once
#include "../glad/glad.h"
#include "glTexture.h"

class glRenderBuffer
{
	GLuint id;
	GLuint texture;
	GLuint latest_bind;

public:
	glRenderBuffer(int width, int height);
	GLuint getID(void);
	~glRenderBuffer();
};