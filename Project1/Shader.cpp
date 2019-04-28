#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>


Shader::Shader(const std::string shader_file, GLuint _type)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string code;
	std::ifstream file;
	// ensure ifstream objects can throw exceptions:
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		file.open(shader_file);
		std::stringstream vShaderStream;
		// read file's buffer contents into streams
		vShaderStream << file.rdbuf();
		// close file handlers
		file.close();
		// convert stream into string
		code = vShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	source = code;
	type = _type;
	id = glCreateShader(_type);

	const char *cc = code.c_str();
	glShaderSource(id, 1, &cc, NULL);
	glCompileShader(id);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

const std::string Shader::get_source(void)
{
	return source;
}

GLuint Shader::get_type(void)
{
	return type;
}

Shader::~Shader()
{
	glDeleteShader(id);
}
