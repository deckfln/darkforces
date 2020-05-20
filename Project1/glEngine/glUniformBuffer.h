#pragma once

#include "glProgram.h"

constexpr auto GL_UBO = 999999;

class glUniformBuffer
{
	unsigned int id;
	unsigned int bindingPoint;
	GLint m_offset;


public:
	glUniformBuffer(int size);
	void bind(glProgram *program, std::string name);
	void bind(void);
	void map(void *data, int offset, int size);
	void unbind(void);
	~glUniformBuffer();
};