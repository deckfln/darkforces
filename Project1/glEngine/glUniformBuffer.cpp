#include "glUniformBuffer.h"

#include "../glad/glad.h"

static unsigned int maxBindingPoint = 0;

glUniformBuffer::glUniformBuffer(int size)
{
	glGenBuffers(1, &id);

	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, maxBindingPoint, id, 0, size);

	bindingPoint = maxBindingPoint;
	maxBindingPoint++;
}

void glUniformBuffer::bind(glProgram *program, std::string name)
{
	program->bindBufferAttribute(name, bindingPoint);
}

void glUniformBuffer::bind(void)
{
	glBindBuffer(GL_UNIFORM_BUFFER, id);
}

void glUniformBuffer::map(void *data, int offset, int size)
{
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void glUniformBuffer::unbind(void)
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glUniformBuffer::~glUniformBuffer()
{
	glDeleteBuffers(1, &id);
	maxBindingPoint--;
}
