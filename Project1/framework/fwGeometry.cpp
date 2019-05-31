#include "fwGeometry.h"

#include <glm/gtx/norm.hpp>
#include <math.h>

#include "math/fwBox3.h"

#include "glEngine/glVertexAttribute.h"
#include "glEngine/glBufferAttribute.h"
#include "../glad/glad.h"


fwGeometry::fwGeometry()
{
	vertices = nullptr;
	index = nullptr;
}

fwGeometry&  fwGeometry::addVertices(std::string _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit)
{
	vertices = new glBufferAttribute(_name, GL_ARRAY_BUFFER, _data, itemSize, len, _sizeof_element, delete_on_exit);
	indexedGeometry = false;

	return *this;
}

fwGeometry&  fwGeometry::addIndex(void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit)
{
	index = new glBufferAttribute("_index", GL_ELEMENT_ARRAY_BUFFER, _data, itemSize, len, _sizeof_element, delete_on_exit);
	indexedGeometry = true;

	return *this;
}

fwGeometry& fwGeometry::addAttribute(std::string _name, GLuint _type, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit)
{
	attributes[current_attribute] = new	glBufferAttribute(_name, _type, _data, itemSize, len, _sizeof_element, delete_on_exit);
	current_attribute++;

	return *this;
}

void fwGeometry::enable_attributes(glProgram *program)
{
	// vertex attribute
	if (index != nullptr) {
		index->bind();
	}
	glVertexAttribute *va = program->get_attribute(vertices->get_name());
	if (va) {
		va->EnableVertex(vertices);
	}

	// other attributes
	glBufferAttribute *attribute;
	for (int i = 0; i < current_attribute; i++) {
		attribute = attributes[i];
		glVertexAttribute *va = program->get_attribute(attribute->get_name());
		if (va) {
			va->EnableVertex(attribute);
		}
	}
}

int fwGeometry::get_count(void)
{
	return count;
}

void fwGeometry::draw(GLenum mode, glVertexArray *va)
{
	if (index != nullptr) {
		va->draw(mode, true, index->get_count());
	}
	else {
		va->draw(mode, false, vertices->get_count());
	}
}

fwSphere *fwGeometry::computeBoundingsphere(void)
{
	fwBox3 box;
	if (m_pBoundingsphere == nullptr) {
		m_pBoundingsphere = new fwSphere();
	}

	if (vertices) {
		glm::vec3 &center = m_pBoundingsphere->center();
		box.setFromBufferAttribute(vertices);
		box.get_center(center);
		
		// hoping to find a boundingSphere with a radius smaller than the
		// boundingSphere of the boundingBox: sqrt(3) smaller in the best case

		float maxRadiusSq = 0;
		for (unsigned int i = 0; i < vertices->get_count(); ++i) {
			glm::vec3 *v = (glm::vec3 *)vertices->get_index(i);
			maxRadiusSq = fmax(maxRadiusSq, glm::distance2(center, *v));
		}

		m_pBoundingsphere->radius(sqrt(maxRadiusSq));
	}

	return m_pBoundingsphere;
}

fwGeometry::~fwGeometry()
{
	if (index != nullptr) {
		delete index;
	}

	delete vertices;

	for (int i = 0; i < current_attribute; i++) {
		delete attributes[current_attribute];
	}
}