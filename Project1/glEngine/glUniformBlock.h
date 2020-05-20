#pragma once

#include <string>
#include <map>
#include "../glad/glad.h"

#include "glUniformBlock.h"

class glUniform;

class glUniformBlock {
	std::string m_name;
	GLint m_size;
	std::map<std::string, glUniform*> m_uniforms;

public:
	glUniformBlock(GLchar* name, int program, int index);
	~glUniformBlock();
};