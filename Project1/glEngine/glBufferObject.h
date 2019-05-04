#pragma once
#include "glad/glad.h"
#include <iostream>

class glBufferObject
{
	GLuint buffer;
	GLenum type;

public:
	glBufferObject(void);
	glBufferObject(GLuint _type, GLuint size, void *data);
	void bind(void);
	static void unbind(void);
	~glBufferObject();
};
