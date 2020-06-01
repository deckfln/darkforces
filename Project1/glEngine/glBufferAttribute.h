#pragma once
#include <iostream>
#include "../glad/glad.h"
#include "glBufferObject.h"

#define ARRAY_SIZE_OF_ELEMENT(array) sizeof(array[0])
#define ARRAY_NB_ELEMENTS(array) sizeof(array)/sizeof(array[0])

class glBufferAttribute
{
protected:
	const std::string m_name;
	GLuint m_type;
	void *m_data;
	GLsizei m_len;					// size of the buffer in bytes
	GLuint m_itemSize;				// number of component in an item
	GLuint m_count;					// number of items
	GLuint m_sizeof_element;		// size of a single element in bytes

	bool m_delete_on_exit = true;	// delete data on exit

	glBufferObject *m_vbo;

	bool m_dirty = false;		// the attribute need to be uploaded to the GPU

public:
	glBufferAttribute();
	glBufferAttribute(const std::string _name, GLuint _type, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);
	GLuint get_size(void);
	GLvoid *data();
	GLuint get_elements(void);
	GLuint type(void);
	const GLuint count(void);
	const std::string& name(void);
	void dirty(void) { m_dirty = true; };		// mark the attribute to be uploaded to the GPU
	void *get_index(const int index);
	void update(int offset = 0, int size = -1);
	void bind();
	void updateIfDirty(void);					// upload the whole buffer to the GPU if needed
	virtual int get_divisor(void) { return 0; };

	~glBufferAttribute();
};
