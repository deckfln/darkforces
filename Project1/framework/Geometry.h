#pragma once
#include <iostream>
#include <map>
#include <iostream>

#include "glEngine/glProgram.h"
#include "../Reference.h"

class Geometry: public Reference
{
	glBufferAttribute *index;
	glBufferAttribute *vertices;

	int count;
	bool indexedGeometry;

	int max_attributes = 10;
	int current_attribute = 0;
	glBufferAttribute *attributes[10];

public:
	Geometry();
	Geometry& addVertices(std::string _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element);
	Geometry& addIndex(void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element);
	Geometry& addAttribute(std::string _name, GLuint _type, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element);
	void enable_attributes(glProgram *program);
	int get_count(void);
	void draw(GLenum mode);
	~Geometry();
};
