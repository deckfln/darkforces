#include "glUniform.h"
#include <iostream>


glUniform::glUniform()
{
}

glUniform::glUniform(GLchar *_name, GLsizei _length, GLsizei _size, GLenum _type, GLint _location)
{
	name = std::string(_name);
	length = _length;
	size = _size;
	type = _type;
	location = _location;

	switch (type) {
	case GL_INT:
		single_type = GL_INT;
		single_size = sizeof(GLint);
		single_nb = 1;
		break;
	case GL_FLOAT:
		single_type = GL_FLOAT;
		single_size = sizeof(GLfloat);
		single_nb = 1;
		break;
	case GL_FLOAT_VEC2:
		single_type = GL_FLOAT;
		single_size = sizeof(GLfloat);
		single_nb = 2;
		break;
	case GL_FLOAT_VEC3:
		single_type = GL_FLOAT;
		single_size = sizeof(GLfloat);
		single_nb = 3;
		break;
	case GL_FLOAT_VEC4:
		single_type = GL_FLOAT;
		single_size = sizeof(GLfloat);
		single_nb = 4;
		break;
	case GL_FLOAT_MAT4:
		single_type = GL_FLOAT;
		single_size = 16*sizeof(GLfloat);
		single_nb = 4;
		break;
	case GL_SAMPLER_2D:
		single_type = GL_INT;
		single_size = sizeof(GLint);
		single_nb = 1;
		break;
	}
}

GLint glUniform::get_location(void)
{
	return location;
}

const std::string glUniform::get_name(void)
{
	return name;
}

void glUniform::set_table(void)
{
	/*
	switch (type) {
	case GL_FLOAT:
		glUniform1f(location, r);
		break;
	case GL_FLOAT_VEC2:
		glUniform2f(location, r, g);
		break;
	case GL_FLOAT_VEC3:
		glUniform3f(location, r, g, b);
		break;
	case GL_FLOAT_VEC4:
		glUniform4f(location, r, g, b, a);
		// glUniform4fv(location, [r, g, b, a]);
		break;
	case GL_FLOAT_MAT2:
		glUniformMatrix2fv(location, 1, GL_FALSE, [r, g]);
		break;
	case GL_FLOAT_MAT3:
		glUniformMatrix3fv(location, 1, GL_FALSE, [r, g, b]);
		break;
	case GL_FLOAT_MAT4:
		glUniformMatrix4fv(location, 1, GL_FALSE, [r, g, b, a]);
		break;
	case GL_BOOL:
	case GL_INT:
		glUniform1i(location, r);
		break;
	case GL_SAMPLER_2D:
		break;
	}
	*/
}

void glUniform::set_value(GLint id)
{
	if (type != GL_SAMPLER_2D) {
		std::cout << "glUniform::set_value " << name.c_str() << " not G_SAMPLER_2D" << std::endl;
		exit(-1);
	}

	glUniform1i(location, id);
}

void glUniform::set_value(GLfloat f)
{
	if (type != GL_FLOAT) {
		std::cout << "glUniform::set_value " << name.c_str() << " not G_FLOAT" << std::endl;
		exit(-1);
	}

	glUniform1f(location, f);
}

void glUniform::set_value(GLfloat r, GLfloat g, GLfloat b)
{
	if (type != GL_FLOAT_VEC3) {
		std::cout << "glUniform::set_value " << name.c_str() << " not G_FLOAT_VEC3" << std::endl;
		exit(-1);
	}

	glUniform3f(location, r, g, b);
}

void glUniform::set_value(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	if (type != GL_FLOAT_VEC4) {
		std::cout << "glUniform::set_value " << name.c_str() << " not G_FLOAT_VEC4" << std::endl;
		exit(-1);
	}

	glUniform4f(location, r, g, b, a);
}

void glUniform::set_value(glm::vec4 &vec4)
{
	if (type != GL_FLOAT_VEC4) {
		std::cout << "glUniform::set_value " << name.c_str() << " not G_FLOAT_VEC4" << std::endl;
		exit(-1);
	}

	glUniform4fv(location, 1, glm::value_ptr(vec4));
}

void glUniform::set_value(glm::vec3 &vec3)
{
	if (type != GL_FLOAT_VEC3) {
		std::cout << "glUniform::set_value " << name.c_str() << " not G_FLOAT_VEC3" << std::endl;
		exit(-1);
	}

	glUniform3fv(location, 1, glm::value_ptr(vec3));
}

void glUniform::set_value(glm::mat4 &mat4)
{
	if (type != GL_FLOAT_MAT4) {
		std::cout << "glUniform::set_value " << name.c_str() << " not G_FLOAT_MAT4" << std::endl;
		exit(-1);
	}

	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat4));
}

glUniform::~glUniform()
{
}
