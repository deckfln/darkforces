#pragma once
#include "glad/glad.h"
#include <iostream>
#include "glBufferAttribute.h"

class glVertexAttribute
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
	glVertexAttribute(void);
	glVertexAttribute(GLchar *name, GLsizei length, GLsizei size, GLenum type, GLint location);
	GLint get_location(void);
	const std::string get_name(void);
	void EnableVertex(glBufferAttribute *vba);
	~glVertexAttribute();
};
