#include "glShader.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


glShader::glShader(const std::string code, const std::string defines, GLuint _type)
{
	source = code;
	int hasDefines = source.find("#define DEFINES");
	if (hasDefines >= 0) {
		source.replace(hasDefines, sizeof("#define DEFINES"), defines);
	}
	type = _type;
	id = glCreateShader(_type);

	const char *cc = source.c_str();
	glShaderSource(id, 1, &cc, NULL);
	glCompileShader(id);

	// check for shader compile errors
	int success;
	char infoLog[4096];
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(id, 4096, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		std::istringstream in(cc);
		std::string to;
		int line = 1;
		while (std::getline(in, to, '\n')) {
			std::cout << std::to_string(line) << ":" << to << std::endl;
			line++;
		}

		exit(-1);
	}
}

const std::string glShader::get_source(void)
{
	return source;
}

GLuint glShader::get_type(void)
{
	return type;
}

glShader::~glShader()
{
	glDeleteShader(id);
}
