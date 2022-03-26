#include "glVertexAttribute.h"

#include <glm/glm.hpp>
#include "glBufferAttribute.h"

glVertexAttribute::glVertexAttribute(void)
{
}

glVertexAttribute::glVertexAttribute(GLchar *_name, GLsizei _length, GLsizei _size, GLenum _type, GLint _location):
	m_name(std::string(_name)),
	m_length(_length),
	m_size(_size),
	m_type(_type),
	m_location(_location)
{

	switch (m_type) {
	case GL_FLOAT:
		m_single_type = GL_FLOAT;
		m_single_size = sizeof(GLfloat);
		m_single_nb = 1;
		break;
	case GL_FLOAT_VEC2:
		m_single_type = GL_FLOAT;
		m_single_size = sizeof(GLfloat);
		m_single_nb = 2;
		break;
	case GL_FLOAT_VEC3:
	case GL_FLOAT_MAT3:
		m_single_type = GL_FLOAT;
		m_single_size = sizeof(GLfloat);
		m_single_nb = 3;
		break;
	case GL_FLOAT_VEC4:
	case GL_FLOAT_MAT4:
		m_single_type = GL_FLOAT;
		m_single_size = sizeof(GLfloat);
		m_single_nb = 4;
		break;
	case GL_INT_VEC4:
		m_single_type = GL_INT;
		m_single_size = sizeof(GLint);
		m_single_nb = 4;
		break;
	case GL_UNSIGNED_INT:
		m_single_type = GL_UNSIGNED_INT;
		m_single_size = sizeof(GLuint);
		m_single_nb = 1;
		break;
	case GL_INT:
		m_single_type = GL_INT;
		m_single_size = sizeof(GLint);
		m_single_nb = 1;
		break;
	default:
		std::cout << "Unkown vertex attribute type " << m_name << " " << m_type << std::endl;
	}
}

GLint glVertexAttribute::location(void)
{
	return m_location;
}

const std::string& glVertexAttribute::name(void)
{
	return m_name;
}

void glVertexAttribute::EnableVertex(glBufferAttribute *vba)
{
	vba->bind();

	int divisor = vba->get_divisor();

	switch (m_type) {
	case GL_FLOAT_MAT4:
		for (int i = 0; i < 4; i++) {
			glVertexAttribPointer(m_location + i, m_single_nb, m_single_type, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
			glEnableVertexAttribArray(m_location + i);

			if (divisor > 0)
				glVertexAttribDivisor(m_location + i, divisor);
		}
		break;
	case GL_INT_VEC4:
		glVertexAttribIPointer(m_location, m_single_nb, m_single_type, 0, (void*)0);
		glEnableVertexAttribArray(m_location);

		if (divisor > 0)
			glVertexAttribDivisor(m_location, divisor);
		break;
	case GL_INT:
		glVertexAttribIPointer(m_location, m_single_nb, m_single_type, 0, (void*)0);
		glEnableVertexAttribArray(m_location);

		if (divisor > 0)
			glVertexAttribDivisor(m_location, divisor);
		break;

	default:
		glVertexAttribPointer(m_location, m_single_nb, m_single_type, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(m_location);

		if (divisor > 0)
			glVertexAttribDivisor(m_location, divisor);
	}

}

glVertexAttribute::~glVertexAttribute()
{
}
