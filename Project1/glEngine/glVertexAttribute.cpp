#include "glVertexAttribute.h"

#include <glm/glm.hpp>

glVertexAttribute::glVertexAttribute(void)
{
}

glVertexAttribute::glVertexAttribute(GLchar *_name, GLsizei _length, GLsizei _size, GLenum _type, GLint _location)
{
	name = std::string(_name);
	length = _length;
	size = _size;
	type = _type;
	location = _location;

	switch (type) {
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
	case GL_FLOAT_MAT3:
		single_type = GL_FLOAT;
		single_size = sizeof(GLfloat);
		single_nb = 3;
		break;
	case GL_FLOAT_VEC4:
	case GL_FLOAT_MAT4:
		single_type = GL_FLOAT;
		single_size = sizeof(GLfloat);
		single_nb = 4;
		break;
	}
}

GLint glVertexAttribute::get_location(void)
{
	return location;
}

const std::string glVertexAttribute::get_name(void)
{
	return name;
}

void glVertexAttribute::EnableVertex(glBufferAttribute *vba)
{
	vba->bind();

	int divisor = vba->get_divisor();

	if (type == GL_FLOAT_MAT4) {
		for (int i = 0; i < 4; i++) {
			glVertexAttribPointer(location + i, single_nb, single_type, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
			glEnableVertexAttribArray(location + i);

			if (divisor > 0)
				glVertexAttribDivisor(location + i, divisor);
		}
	}
	else {
		glVertexAttribPointer(location, single_nb, single_type, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(location);

		if (divisor > 0)
			glVertexAttribDivisor(location, divisor);
	}

}

glVertexAttribute::~glVertexAttribute()
{
}
