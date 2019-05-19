#pragma once
#include <iostream>
#include <map>
#include <iostream>

#include "../Reference.h"
#include "../glEngine/glBufferAttribute.h"
#include "../glEngine/glVertexArray.h"
#include "../glEngine/glProgram.h"

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
	Geometry& addVertices(std::string _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit=true);
	Geometry& addIndex(void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);
	Geometry& addAttribute(std::string _name, GLuint _type, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);
	void enable_attributes(glProgram *program);
	int get_count(void);
	void draw(GLenum mode, glVertexArray *);
	~Geometry();
};
