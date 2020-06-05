#pragma once
#include "../glad/glad.h"
#include <iostream>
class glBufferAttribute;

class glVertexAttribute
{
	std::string m_name;
	GLsizei m_length;
	GLsizei m_size;
	GLenum m_type;
	GLint m_location;

	GLenum m_single_type;
	GLuint m_single_size;
	GLuint m_single_nb;

public:
	glVertexAttribute(void);
	glVertexAttribute(GLchar *name, GLsizei length, GLsizei size, GLenum type, GLint location);
	GLint location(void);
	const std::string& name(void);
	void EnableVertex(glBufferAttribute *vba);
	~glVertexAttribute();
};
