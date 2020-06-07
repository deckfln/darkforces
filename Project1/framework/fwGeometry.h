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
	int m_id = 0;

	glBufferAttribute *index = nullptr;
	glBufferAttribute *vertices = nullptr;

	int m_verticesToDisplay = -1;	// number of vertices to display from the buffers
									// -1 == all vertices
	bool indexedGeometry = false;

	int max_attributes = 10;
	int current_attribute = 0;
	std::map <const std::string, glBufferAttribute *> attributes;

	bool m_dirty = false;	// ALL attributes need to be re uploaded to the GPU

	fwSphere *m_pBoundingsphere = nullptr;

public:
	fwGeometry();
	fwGeometry& addVertices(const std::string _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit=true);
	fwGeometry& addDynamicVertices(const std::string _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);
	fwGeometry& addIndex(void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);
	fwGeometry& addAttribute(const std::string _name, GLuint _type, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);

	void updateVertices(int offset = 0, int size = -1);
	void updateAttribute(const std::string &name, int offset=0, int size = -1);
	void update(void);

	void enable_attributes(glProgram *program);

	int verticesToDisplay(void);
	void verticesToDisplay(int nb);

	void dirty(void) { m_dirty = true; };	// request all attributes to the uploaded to the GPU
	void updateIfDirty(void);				// check if something need to be uploaded to the GPU

	fwSphere *boundingsphere(void) { return m_pBoundingsphere; };
	fwSphere *computeBoundingsphere(void);
	float sqDistance2boundingSphere(glm::vec3& position);
	fwSphere* setBoundingsphere(float radius);
	void computeTangent(void);

	int id(void) { return m_id; };

	void draw(GLenum mode, glVertexArray *);
	~fwGeometry();
};
