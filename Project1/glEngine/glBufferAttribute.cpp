#include "glBufferAttribute.h"

glBufferAttribute::glBufferAttribute()
{

}

glBufferAttribute::glBufferAttribute(const std::string& _name, GLuint _type, void *_data, GLsizei _itemSize, GLsizei _len, GLuint _sizeof_element, bool _delete_on_exit):
	m_name(_name),
	m_type(_type),
	m_data(_data),
	m_len(_len),
	m_itemSize(_itemSize),
	m_sizeof_element(_sizeof_element),
	m_delete_on_exit(_delete_on_exit)
{
	m_count = m_len / (_sizeof_element * m_itemSize);

	m_vbo = new glBufferObject(m_type, m_len, m_data);
}

GLuint glBufferAttribute::get_size(void)
{
	return m_len;
}

GLvoid *glBufferAttribute::data(void)
{
	return m_data;
}

GLuint glBufferAttribute::type(void)
{
	return m_type;
}

const std::string& glBufferAttribute::name(void)
{
	return m_name;
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

const GLuint glBufferAttribute::count(void)
{
	return m_count;
}

void glBufferAttribute::bind()
{
	m_vbo->bind();
}

/**
 * upload the buffer in thr GPU
 */
void glBufferAttribute::update(int offset, int size)
{
	int le = m_sizeof_element * m_itemSize;

	if (size == -1) {
		// update the whole buffer
		size = m_count;
	}
	m_vbo->update(offset * le, size * le, (char *)m_data + offset * le);

	m_dirty = false;
}

/**
 * the owner changed the size and storage buffer
 */
void glBufferAttribute::resize(void* data, int itemCount)
{
	m_count = itemCount;
	m_len = m_count * (m_sizeof_element * m_itemSize);
	m_data = data;

	// allocate a new VBO
	delete m_vbo;
	m_vbo = new glBufferObject(m_type, m_len, m_data);
}

void *glBufferAttribute::get_index(const int index)
{
	int le = m_sizeof_element * m_itemSize;

	return (char *)m_data + index * le;
}

glBufferAttribute::~glBufferAttribute()
{
	if (m_delete_on_exit)
		delete m_data;

	delete m_vbo;
}
