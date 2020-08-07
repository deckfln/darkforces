#pragma once
#include <iostream>
#include <map>
#include <iostream>

#include "../Reference.h"
#include "../glEngine/glBufferAttribute.h"
#include "../glEngine/glVertexArray.h"
#include "../glEngine/glProgram.h"

#include "fwAABBox.h"
#include "math/fwSphere.h"

class fwGeometry: public Reference
{
	int m_id = 0;

	glBufferAttribute *index = nullptr;
	glBufferAttribute *m_vertices = nullptr;

	int m_verticesToDisplay = -1;	// number of vertices to display from the buffers
									// -1 == all vertices
	bool indexedGeometry = false;

	int max_attributes = 10;
	int current_attribute = 0;
	std::map <const std::string, glBufferAttribute *> m_attributes;

	bool m_dirty = false;				// ALL attributes need to be re uploaded to the GPU
	bool m_resizedAttribute = false;	// at least one of the attribute was resized

	fwAABBox m_modelAABB;				// bounding box in model space
	fwSphere *m_pBoundingsphere = nullptr;

public:
	fwGeometry();
	fwGeometry& addVertices(const std::string& _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit=true);
	fwGeometry& addDynamicVertices(const std::string& _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);
	fwGeometry& addIndex(void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);
	fwGeometry& addAttribute(const std::string& _name, GLuint _type, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);

	void updateVertices(int offset = 0, int size = -1);
	void updateAttribute(const std::string& name, int offset=0, int size = -1);
	void resizeAttribute(const std::string& name, 
		void* data, 
		int itemCount);						// the attribute buffer was resized and moved by the owner
	void update(void);

	void enable_attributes(glProgram *program);

	int verticesToDisplay(void);
	void verticesToDisplay(int nb);

	void dirty(void) { m_dirty = true; };	// request all attributes to the uploaded to the GPU
	void updateIfDirty(void);				// check if something need to be uploaded to the GPU
	bool resizedAttribute(void);			// check if at least 1 attribute was resized, reset the flag on exit

	fwSphere *boundingsphere(void) { return m_pBoundingsphere; };
	fwSphere *computeBoundingsphere(void);
	const fwAABBox& aabbox(void);			// return or initialize the model space AABB
	float sqDistance2boundingSphere(const glm::vec3& position);
	fwSphere* setBoundingsphere(float radius);
	void computeTangent(void);

	int id(void) { return m_id; };

	void draw(GLenum mode, glVertexArray *);

	const glm::vec3& center(void);						// get the center of the boundingsphere around the geometry
	const glm::vec3& centerVertices(void);				// move all vertices along the translation

	glm::vec3 const* vertices(void);					// direct access to the vertices
	uint32_t nbvertices(void);							// direct access to the #vertices

	~fwGeometry();
};
