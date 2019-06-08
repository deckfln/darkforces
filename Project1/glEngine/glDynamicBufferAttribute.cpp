#include "glDynamicBufferAttribute.h"

glDynamicBufferAttribute::glDynamicBufferAttribute(const std::string _name, GLuint _type, void *_data, GLsizei _itemSize, GLsizei _len, GLuint _sizeof_element, bool _delete_on_exit):
	glBufferAttribute(_name, _type, _data, _itemSize, _len, _sizeof_element, _delete_on_exit)
{
	delete vbo;
	vbo = new glBufferObject(type, len, data, GL_DYNAMIC_DRAW);
}

glDynamicBufferAttribute::~glDynamicBufferAttribute()
{

}