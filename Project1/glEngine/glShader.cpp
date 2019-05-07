#include "glShader.h"
#include <iostream>
#include <fstream>
#include <sstream>


glShader::glShader(const std::string shader_file, const std::string defines, GLuint _type)
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
		std::cout << "ERROR::SHADER " <<shader_file << " ::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		exit(-1);
	}

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
	char infoLog[512];
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
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
