#include "Geometry.h"
#include "glEngine/glVertexAttribute.h"
#include "glEngine/glBufferAttribute.h"
#include "../glad/glad.h"

Geometry::Geometry()
{
	vertices = nullptr;
	index = nullptr;
}

Geometry&  Geometry::addVertices(std::string _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit)
{
	vertices = new glBufferAttribute(_name, GL_ARRAY_BUFFER, _data, itemSize, len, _sizeof_element, delete_on_exit);
	indexedGeometry = false;

	return *this;
}

Geometry&  Geometry::addIndex(void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit)
{
	index = new glBufferAttribute("_index", GL_ELEMENT_ARRAY_BUFFER, _data, itemSize, len, _sizeof_element, delete_on_exit);
	indexedGeometry = true;

	return *this;
}

Geometry& Geometry::addAttribute(std::string _name, GLuint _type, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit)
{
	attributes[current_attribute] = new	glBufferAttribute(_name, _type, _data, itemSize, len, _sizeof_element, delete_on_exit);
	current_attribute++;

	return *this;
}

void Geometry::enable_attributes(glProgram *program)
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

int Geometry::get_count(void)
{
	return count;
}

void Geometry::draw(GLenum mode, glVertexArray *va)
{
	if (index != nullptr) {
		va->draw(mode, true, index->get_count());
	}
	else {
		va->draw(mode, false, vertices->get_count());
	}
}

Geometry::~Geometry()
{
	if (index != nullptr) {
		delete index;
	}

	delete vertices;

	for (int i = 0; i < current_attribute; i++) {
		delete attributes[current_attribute];
	}
}