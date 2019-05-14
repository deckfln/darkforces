#include "glProgram.h"
#include "glShader.h"
#include <iostream>
#include "List.h"
#include "glad/glad.h"

List Shaders;

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
