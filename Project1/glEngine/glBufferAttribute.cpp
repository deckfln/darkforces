#include "glBufferAttribute.h"

glBufferAttribute::glBufferAttribute()
{

}

glBufferAttribute::glBufferAttribute(const std::string _name, GLuint _type, void *_data, GLsizei _itemSize, GLsizei _len, GLuint _sizeof_element, bool _delete_on_exit):
	name(_name),
	type(_type),
	data(_data),
	len(_len),
	itemSize(_itemSize),
	sizeof_element(_sizeof_element),
	delete_on_exit(_delete_on_exit)
{
	count = len / (_sizeof_element * itemSize);

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

/**
 * Upload the whole bufer to the GPU
 */
void glBufferAttribute::updateIfDirty(void)
{
	if (m_dirty) {
		update();
	}
}

const GLuint glBufferAttribute::get_count(void)
{
	return count;
}

void glBufferAttribute::bind()
{
	vbo->bind();
}

/**
 * upload the buffer in thr GPU
 */
void glBufferAttribute::update(int offset, int size)
{
	int le = sizeof_element * itemSize;

	if (size == -1) {
		// update the whole buffer
		size = count;
	}
	vbo->update(offset * le, size * le, (char *)data + offset * le);

	m_dirty = false;
}

void *glBufferAttribute::get_index(const int index)
{
	int le = sizeof_element * itemSize;

	return (char *)data + index * le;
}

glBufferAttribute::~glBufferAttribute()
{
	if (delete_on_exit)
		delete data;

	delete vbo;
}
