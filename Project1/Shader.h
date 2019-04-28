#pragma once
#include <iostream>
#include "glad/glad.h"
#include "Element.h"

class Shader
{
	std::string source;
	GLuint type;

public:
	GLuint id;

	Shader(const std::string shader_file, GLuint type);
	const std::string get_source(void);
	GLuint get_type(void);
	~Shader();
};
