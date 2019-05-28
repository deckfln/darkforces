#pragma once
#include "glad/glad.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "framework/fwTexture.h"

class glUniform
{
	std::string name;
	GLsizei length;
	GLsizei size;
	GLenum type;
	GLint location;

	GLenum single_type;
	GLuint single_size;
	GLuint single_nb;

public:
	glUniform();
	glUniform(GLchar *name, GLsizei length, GLsizei size, GLenum type, GLint location);
	GLint get_location(void);
	const std::string get_name(void);
	void set_table(void);
	void set_value(GLint id);
	void set_value(GLfloat id);
	void set_value(GLfloat r, GLfloat g, GLfloat b);
	void set_value(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	void set_value(glm::mat4 &mat4);
	void set_value(glm::vec4 &vec4);
	void set_value(glm::vec3 &vec3);
	~glUniform();
};
