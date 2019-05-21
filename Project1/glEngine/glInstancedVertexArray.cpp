#include "glVertexArray.h"
#include "glInstancedVertexArray.h"

glInstancedVertexArray::glInstancedVertexArray(int _instances) :
	instances(_instances)
{

}

void glInstancedVertexArray::set_instances(int _instances)
{
	instances = _instances;
}

void glInstancedVertexArray::draw(GLenum mode, bool indexed, int count)
{
	bind(); 

	if (indexed) {
		glDrawElementsInstanced(mode, count, GL_UNSIGNED_INT, 0, instances);
	}
	else {
		glDrawArraysInstanced(mode, 0, count, instances);
	}

	glVertexArray::unbind();
}