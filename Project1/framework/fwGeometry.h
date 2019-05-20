#pragma once
#include <iostream>
#include <map>
#include <iostream>

#include "../Reference.h"
#include "../glEngine/glBufferAttribute.h"
#include "../glEngine/glVertexArray.h"
#include "../glEngine/glProgram.h"

class fwGeometry: public Reference
{
	glBufferAttribute *index = nullptr;
	glBufferAttribute *vertices = nullptr;

	int count = 0;
	bool indexedGeometry = false;

	int max_attributes = 10;
	int current_attribute = 0;
	glBufferAttribute *attributes[10];

public:
	fwGeometry();
	fwGeometry& addVertices(std::string _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit=true);
	fwGeometry& addIndex(void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);
	fwGeometry& addAttribute(std::string _name, GLuint _type, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);
	void enable_attributes(glProgram *program);
	int get_count(void);
	void draw(GLenum mode, glVertexArray *);
	~fwGeometry();
};
