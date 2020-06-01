#include "glDynamicBufferAttribute.h"

glDynamicBufferAttribute::glDynamicBufferAttribute(const std::string _name, GLuint _type, void *_data, GLsizei _itemSize, GLsizei _len, GLuint _sizeof_element, bool _delete_on_exit):
	glBufferAttribute(_name, _type, _data, _itemSize, _len, _sizeof_element, _delete_on_exit)
{
	delete m_vbo;
	m_vbo = new glBufferObject(m_type, m_len, m_data, GL_DYNAMIC_DRAW);
}

glDynamicBufferAttribute::~glDynamicBufferAttribute()
{

}