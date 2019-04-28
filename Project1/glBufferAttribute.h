#pragma once
#include <iostream>
#include "glad/glad.h"
#include "glBufferObject.h"

#define ARRAY_SIZE_OF_ELEMENT(array) sizeof(array[0])
#define ARRAY_NB_ELEMENTS(array) sizeof(array)/sizeof(array[0])

class glBufferAttribute
{
	std::string name;
	GLuint type;
	void *data;
	GLsizei len;
	GLuint itemSize;
	GLuint count;
	GLuint sizeof_element;
	
	glBufferObject *vbo;

public:
	glBufferAttribute(std::string _name, GLuint _type, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element);
	GLuint get_size(void);
	GLvoid *get_data();
	GLuint get_elements(void);
	GLuint get_type(void);
	const GLuint get_count(void);
	void bind();
	const std::string get_name(void);
	~glBufferAttribute();
};
