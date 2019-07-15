#pragma once
#include "../glad/glad.h"
#include <iostream>

class glBufferObject
{
	GLuint buffer;
	GLenum type;

public:
	glBufferObject(void);
	glBufferObject(GLuint _type, GLuint size, void *data, int flag=GL_STATIC_DRAW);
	void bind(void);
	void update(int offset, int size, void *data);
	static void unbind(void);
	~glBufferObject();
};
