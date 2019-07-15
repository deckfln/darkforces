#pragma once
#include <iostream>
#include "../glad/glad.h"

class glShader
{
	std::string source;
	GLuint type;

public:
	GLuint id;

	glShader(const std::string shader_file, const std::string defines, GLuint type);
	const std::string get_source(void);
	GLuint get_type(void);
	~glShader();
};
