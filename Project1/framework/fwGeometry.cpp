#include "fwGeometry.h"

#include <glm/gtx/norm.hpp>
#include <math.h>
#include <exception>

#include "math/fwBox3.h"

#include "../glEngine/glVertexAttribute.h"
#include "../glEngine/glBufferAttribute.h"
#include "../glad/glad.h"


fwGeometry::fwGeometry()
{
	vertices = nullptr;
	index = nullptr;
}

fwGeometry& fwGeometry::addVertices(const std::string _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit)
{
	vertices = new glBufferAttribute(_name, GL_ARRAY_BUFFER, _data, itemSize, len, _sizeof_element, delete_on_exit);
	indexedGeometry = false;

	return *this;
}

fwGeometry& fwGeometry::addDynamicVertices(const std::string _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit)
{
	vertices = new glBufferAttribute(_name, GL_ARRAY_BUFFER, _data, itemSize, len, _sizeof_element, delete_on_exit);
	indexedGeometry = false;

	return *this;
}

fwGeometry& fwGeometry::addIndex(void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit)
{
	index = new glBufferAttribute("_index", GL_ELEMENT_ARRAY_BUFFER, _data, itemSize, len, _sizeof_element, delete_on_exit);
	indexedGeometry = true;

	return *this;
}

fwGeometry& fwGeometry::addAttribute(const std::string name, GLuint type, void *data, GLsizei itemSize, GLsizei len, GLuint sizeof_element, bool delete_on_exit)
{
	glBufferAttribute *ba = new	glBufferAttribute(name, type, data, itemSize, len, sizeof_element, delete_on_exit);
	std::pair<const std::string, glBufferAttribute *> in(name, ba);
	attributes.insert(in);

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
	for (auto attribute: attributes) {
		const std::string _name = attribute.first;
		glBufferAttribute *_attribute = attribute.second;
		glVertexAttribute *va = program->get_attribute(_attribute->get_name());
		if (va) {
			va->EnableVertex(_attribute);
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

/**
 * parse all vertices to compute a bounding sphere
 */
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

/**
 * compute aTangent for normalmaps
 */
void fwGeometry::computeTangent(void)
{
	glm::vec3 *v0;
	glm::vec3 *v1;
	glm::vec3 *v2;

	glm::vec2 *uv0;
	glm::vec2 *uv1;
	glm::vec2 *uv2;

	glm::vec3 Edge1;
	glm::vec3 Edge2;

	float DeltaU1;
	float DeltaV1;
	float DeltaU2;
	float DeltaV2;

	float f;

	glm::vec3 Tangent;

	glBufferAttribute *uvs = attributes["aTexCoord"];

	int nb_vertices = vertices->get_count();
	glm::vec3 *tangents = new glm::vec3 [nb_vertices];
	glm::vec3 *_vertices = (glm::vec3 *)vertices->get_data();
	glm::vec2 *_uv = (glm::vec2 *)uvs->get_data();

	if (index == nullptr) {
		for (int i = 0; i < nb_vertices; i += 3) {
			v0 = _vertices + i;
			v1 = _vertices + (i + 1);
			v2 = _vertices + (i + 2);
		
			uv0 = _uv + i;
			uv1 = _uv + (i + 1);
			uv2 = _uv + (i + 2);

			Edge1 = *v1 - *v0;
			Edge2 = *v2 - *v0;

			DeltaU1 = uv1->x - v0->x;
			DeltaV1 = uv1->y - uv0->y;
			DeltaU2 = uv2->x - uv0->x;
			DeltaV2 = uv2->y - uv0->y;

			f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

			Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
			Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
			Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);
			tangents[i] += Tangent;
			tangents[i + 1] += Tangent;
			tangents[i + 2] += Tangent;
		}
	}
	else {
		GLint *_index = (GLint *)index->get_data();
		int k0, k1, k2;
		for (int i = 0; i < index->get_count(); i += 3) {
			k0 = _index[i];
			k1 = _index[i + 1];
			k2 = _index[i + 2];

			v0 = _vertices + k0;
			v1 = _vertices + k1;
			v2 = _vertices + k2;

			uv0 = _uv + k0;
			uv1 = _uv + k1;
			uv2 = _uv + k2;

			Edge1 = *v1 - *v0;
			Edge2 = *v2 - *v0;

			DeltaU1 = uv1->x - v0->x;
			DeltaV1 = uv1->y - uv0->y;
			DeltaU2 = uv2->x - uv0->x;
			DeltaV2 = uv2->y - uv0->y;

			f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

			Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
			Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
			Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

			tangents[k0] += Tangent;
			tangents[k1] += Tangent;
			tangents[k2] += Tangent;
		}

	}

	for (int i = 0; i < nb_vertices; i++) {
		tangents[i] = glm::normalize(tangents[i]);
	}

	addAttribute("tangent", GL_ARRAY_BUFFER, tangents, 3, nb_vertices * sizeof(glm::vec3), sizeof(float), true);
}

/**
 * destructor
 */
fwGeometry::~fwGeometry()
{
	if (index != nullptr) {
		delete index;
	}

	delete vertices;

	for (auto attribute: attributes) {
		delete attribute.second;
	}
}