#include "glInstancedBufferAttribute.h"

glInstancedBufferAttribute::glInstancedBufferAttribute(std::string _name, int _divisor, GLuint _type, void *_data, GLsizei _itemSize, GLsizei _len, GLuint _sizeof_element, bool _delete_on_exit):
	glDynamicBufferAttribute(_name, _type, _data, _itemSize, _len, _sizeof_element, _delete_on_exit),
	divisor(_divisor)
{
}
