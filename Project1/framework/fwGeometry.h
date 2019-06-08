#pragma once
#include <iostream>
#include <map>
#include <iostream>

#include "../Reference.h"
#include "../glEngine/glBufferAttribute.h"
#include "../glEngine/glVertexArray.h"
#include "../glEngine/glProgram.h"
#include "math/fwSphere.h"

class fwGeometry: public Reference
{
	glBufferAttribute *index = nullptr;
	glBufferAttribute *vertices = nullptr;

	int count = 0;
	bool indexedGeometry = false;

	int max_attributes = 10;
	int current_attribute = 0;
	std::map <const std::string, glBufferAttribute *> attributes;

	fwSphere *m_pBoundingsphere = nullptr;

public:
	fwGeometry();
	fwGeometry& addVertices(const std::string _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit=true);
	fwGeometry& addDynamicVertices(const std::string _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);
	fwGeometry& addIndex(void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);
	fwGeometry& addAttribute(const std::string _name, GLuint _type, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);
	
	void enable_attributes(glProgram *program);
	int get_count(void);

	fwSphere *boundingsphere(void) { return m_pBoundingsphere; };
	fwSphere *computeBoundingsphere(void);
	void computeTangent(void);

	void draw(GLenum mode, glVertexArray *);
	~fwGeometry();
};
