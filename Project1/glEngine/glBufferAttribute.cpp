#include "glBufferAttribute.h"

glBufferAttribute::glBufferAttribute(std::string _name, GLuint _type, void *_data, GLsizei _itemSize, GLsizei _len, GLuint _sizeof_element)
{
	name = _name;
	type = _type;
	data  =_data;
	len = _len;
	itemSize = _itemSize;
	count = len / (_sizeof_element * itemSize);
	sizeof_element = _sizeof_element;

	vbo = new glBufferObject(type, len, data);
}

GLuint glBufferAttribute::get_size(void)
{
	return len;
}

GLvoid *glBufferAttribute::get_data(void)
{
	return data;
}

GLuint glBufferAttribute::get_type(void)
{
	return type;
}

const std::string glBufferAttribute::get_name(void)
{
	return name;
}

const GLuint glBufferAttribute::get_count(void)
{
	return count;
}

void glBufferAttribute::bind()
{
	vbo->bind();
}

glBufferAttribute::~glBufferAttribute()
{
	free(data);
	delete vbo;
}
