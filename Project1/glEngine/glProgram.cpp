#include "glProgram.h"

#include <iostream>
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "glShader.h"

#include "List.h"
#include "glad/glad.h"

List Shaders;

std::string load_shader_file(const std::string shader_file, std::string defines)
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
		std::cout << "ERROR::SHADER " << shader_file << " ::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		exit(-1);
	}

	// deal with all includes
	const std::regex re_basename("(.*)/");
	std::smatch base_match;
	std::string path = "";

	if (std::regex_search(shader_file, base_match, re_basename)) {
		path = base_match[1].str();
	}

	const std::regex re("#include \"([^\"]*)\"");

	int hasInclude = 1;
	while ((hasInclude = code.find("#include")) >= 0) {
		if (std::regex_search(code, base_match, re)) {
			// The first sub_match is the whole string; the next
			// sub_match is the first parenthesized expression.
			if (base_match.size() == 2) {
				std::string line = base_match[0].str();
				std::string file = base_match[1].str();

				std::string include = load_shader_file(path + "/" + file, defines);
				code.replace(hasInclude, sizeof(line) + 2, include);
			}
		}
	}

	// deal with provided defines

	int hasDefines = code.find("#define DEFINES");
	if (hasDefines >= 0) {
		std::string _defines = defines + "\n#define DEFINES\n";
		code.replace(hasDefines, sizeof("#define DEFINES"), _defines);
	}

	return code;
}

glProgram::glProgram(void)
{
}

glProgram::glProgram(const std::string vertexShader, const std::string fragmentShader, const std::string defines)
{
	glShader *vertex = new glShader(vertexShader, defines, GL_VERTEX_SHADER);
	glShader *fragment = new glShader(fragmentShader, defines, GL_FRAGMENT_SHADER);

	// link shaders
	id = glCreateProgram();

	glAttachShader(id, vertex->id);
	glAttachShader(id, fragment->id);
	glLinkProgram(id);

	// check for linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		exit(-1);
	}

	// extract active attributes
	GLint n;
	GLchar name[255];
	GLsizei length;
	GLsizei size;
	GLenum type;
	GLint location;

	glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &n);
	for (int i = 0; i < n; i++) {
		glGetActiveAttrib(id, i, sizeof(name), &length, &size, &type, name);
		location = glGetAttribLocation(id, name);

		glVertexAttribute *attr = new glVertexAttribute(name, length, size, type, location);
		attributes[name] = attr;
	}

	// extract active uniforms
	glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &n);
	for (int i = 0; i < n; i++) {
		glGetActiveUniform(id, i, sizeof(name), &length, &size, &type, name);
		location = glGetUniformLocation(id, name);

		glUniform *uniform = new glUniform(name, length, size, type, location);
		uniforms[name] = uniform;
	}
}

GLuint glProgram::getID(void)
{
	return id;
}

void glProgram::run(void)
{
	glUseProgram(id);
}

glVertexAttribute *glProgram::get_attribute(const std::string name)
{
	glVertexAttribute *attr = attributes[name];
	if (!attr) {
		//std::cout << "glProgram::get_attribute " << name.c_str() << " mising" << std::endl;
		return NULL;
	}
	return attr;
}

bool glProgram::bindBufferAttribute(std::string name, int bindingPoint)
{
	if (uniformBufferBindingPoints.count(name) == 0) {
		GLuint blockIndex = glGetUniformBlockIndex(id, name.c_str());

		if (blockIndex >= 0) {
			glUniformBlockBinding(id, blockIndex, bindingPoint);
		}

		uniformBufferBindingPoints[name] = blockIndex;
		return true;
	}

	return false;
}

glUniform *glProgram::get_uniform(std::string name)
{
	glUniform *attr = uniforms[name];
	if (!attr) {
		//std::cout << "glProgram::get_uniform " << name.c_str() << " mising" << std::endl;
	}
	return attr;
}

void glProgram::set_uniform(const std::string name, GLint id)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(id);
}

void glProgram::set_uniform(const std::string name, GLfloat f)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(f);
}

void glProgram::set_uniform(const std::string name, GLfloat r, GLfloat g, GLfloat b)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(r, g, b);
}

void glProgram::set_uniform(const std::string name, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(r, g, b, a);
}

void glProgram::set_uniform(const std::string name, glm::mat4 &mat4)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(mat4);
}

void glProgram::set_uniform(const std::string name, glm::vec4 &vec4)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(vec4);
}

void glProgram::set_uniform(const std::string name, glm::vec3 &vec3)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(vec3);
}

glProgram::~glProgram()
{
	glDeleteProgram(id);
}
