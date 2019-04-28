#include "glBufferObject.h"

glBufferObject::glBufferObject(void)
{
}

glBufferObject::glBufferObject(GLuint _type, GLuint size, void *data)
{
	type = _type;
	glGenBuffers(1, &buffer);

	glBindBuffer(type, buffer);
	glBufferData(type, size, data, GL_STATIC_DRAW);
}

void glBufferObject::bind(void)
{
	glBindBuffer(type, buffer);
}

void glBufferObject::unbind(void)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

glBufferObject::~glBufferObject()
{
	glDeleteBuffers(1, &buffer);
}
