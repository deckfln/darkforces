#include "fwGeometry.h"

#include <glm/gtx/norm.hpp>
#include <math.h>
#include <exception>

#include "math/fwBox3.h"

#include "../glEngine/glVertexAttribute.h"
#include "../glEngine/glBufferAttribute.h"
#include "../glEngine/glDynamicBufferAttribute.h"
#include "../glad/glad.h"

static int g_nbGeometries = 0;

fwGeometry::fwGeometry()
{
	vertices = nullptr;
	index = nullptr;
	m_id = g_nbGeometries++;
}

fwGeometry& fwGeometry::addVertices(const std::string _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit)
{
	vertices = new glBufferAttribute(_name, GL_ARRAY_BUFFER, _data, itemSize, len, _sizeof_element, delete_on_exit);
	indexedGeometry = false;
	m_attributes[_name] = vertices;

	return *this;
}

fwGeometry& fwGeometry::addDynamicVertices(const std::string _name, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit)
{
	vertices = new glDynamicBufferAttribute(_name, GL_ARRAY_BUFFER, _data, itemSize, len, _sizeof_element, delete_on_exit);
	indexedGeometry = false;
	m_attributes[_name] = vertices;

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
	m_attributes.insert(in);

	return *this;
}

void fwGeometry::enable_attributes(glProgram *program)
{
	// vertex index attribute
	if (index != nullptr) {
		index->bind();
	}

	// other attributes
	for (auto attribute: m_attributes) {
		const std::string _name = attribute.first;
		glBufferAttribute *_attribute = attribute.second;
		glVertexAttribute *va = program->get_attribute(_attribute->name());
		if (va) {
			va->EnableVertex(_attribute);
		}
	}
}

/*
 * force a dirty attribute to be uploaded to the GPU
 */
void fwGeometry::updateVertices(int offset, int size)
{
	vertices->update(offset, size);
}

void fwGeometry::updateAttribute(const std::string& name, int offset, int size)
{
	glBufferAttribute* attribute = m_attributes[name];
	if (attribute) {
		attribute->update(offset, size);
	}
}

/**
 * the attribute buffer was resized and moved by the owner
 */
void fwGeometry::resizeAttribute(const std::string& name, void* data, int itemCount)
{
	// ensure the attribute exists
	if (m_attributes.count(name) > 0) {
		m_attributes[name]->resize(data, itemCount);
		m_resizedAttribute = true;
	}
}

/**
 * Upload all attributes to the GPU based on the toDisplay
 */
void fwGeometry::update(void)
{
	if (m_verticesToDisplay > 0) {
		// limited number of vertices
		vertices->update(0, m_verticesToDisplay);

		for (auto attribute : m_attributes) {
			attribute.second->update(0, m_verticesToDisplay);
		}
	}
	else {
		// all of them
		for (auto attribute : m_attributes) {
			attribute.second->update();
		}
	}
	m_dirty = false;
}

/**
 * get number of vertices to display (lower than the vertices in the buffer)
 */
int fwGeometry::verticesToDisplay(void)
{
	return m_verticesToDisplay;
}

/**
 * Force number of vertices to display (must be lower than the vertices in the buffer)
 */
void fwGeometry::verticesToDisplay(int nb)
{
	int nbVertices;
	if (index != nullptr) {
		nbVertices = index->count();
	}
	else {
		nbVertices = vertices->count();
	}

	if (nb < nbVertices) {
		m_verticesToDisplay = nb;
	}
	else {
		std::cerr << "fwGeometry::count try to display more vertices than the buffer contains" << std::endl;
	}
}

/**
 * update all attributes on the GPU if the geometry is dirty
 */
void fwGeometry::updateIfDirty(void)
{
	// the whole geometry need to be uploaded ?
	if (m_dirty) {
		update();
	}
	else {
		// and test each of the attibute individualy
		vertices->updateIfDirty();
		for (auto attribute : m_attributes) {
			attribute.second->updateIfDirty();
		}
	}
}

/**
 * check if at least 1 attribute was resized, reset the flag on exit
 */
bool fwGeometry::resizedAttribute(void)
{
	if (m_resizedAttribute) {
		m_resizedAttribute = false;
		return true;
	}
	return false;
}

/**
 * draw call to display the geometry
 */
void fwGeometry::draw(GLenum mode, glVertexArray *va)
{
	if (m_verticesToDisplay > 0) {
		if (index != nullptr) {
			va->draw(mode, true, m_verticesToDisplay);
		}
		else {
			va->draw(mode, false, m_verticesToDisplay);
		}
	}
	else {
		if (index != nullptr) {
			va->draw(mode, true, index->count());
		}
		else {
			va->draw(mode, false, vertices->count());
		}
	}
}

/**
 * Return the center of the vertices
 *   actualy the center of the bounding sphere
 */
const glm::vec3& fwGeometry::center(void)
{
	if (m_pBoundingsphere == nullptr) {
		computeBoundingsphere();
	}

	return m_pBoundingsphere->center();
}

/**
 * Translate all vertices to use center as (0,0,0)
 */
const glm::vec3& fwGeometry::centerVertices(void)
{
	glm::vec3* _vertices = (glm::vec3*)vertices->data();

	// compute the bounding sphere (to find the center)
	if (m_pBoundingsphere == nullptr) {
		computeBoundingsphere();
	}

	const glm::vec3& center = m_pBoundingsphere->center();

	for (auto i = 0; i < vertices->count(); i++) {
		*(_vertices++) -= center;
	}

	return center;
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
		box.setFromBufferAttribute(vertices);
		const glm::vec3& center = box.center();
		m_pBoundingsphere->center(center);

		// hoping to find a boundingSphere with a radius smaller than the
		// boundingSphere of the boundingBox: sqrt(3) smaller in the best case

		float maxRadiusSq = 0;
		for (unsigned int i = 0; i < vertices->count(); ++i) {
			glm::vec3 *v = (glm::vec3 *)vertices->get_index(i);
			maxRadiusSq = fmax(maxRadiusSq, glm::distance2(center, *v));
		}

		m_pBoundingsphere->radius(sqrt(maxRadiusSq));
	}

	return m_pBoundingsphere;
}

float fwGeometry::sqDistance2boundingSphere(glm::vec3& position)
{
	assert(m_pBoundingsphere != nullptr);
	float debug = glm::distance2(m_pBoundingsphere->center(), position);
	return debug;
}

/**
 * parse all vertices to compute a bounding sphere
 */
fwSphere* fwGeometry::setBoundingsphere(float radius)
{
	if (m_pBoundingsphere == nullptr) {
		m_pBoundingsphere = new fwSphere(radius);
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

	glBufferAttribute *uvs = m_attributes["aTexCoord"];

	int nb_vertices = vertices->count();
	glm::vec3 *tangents = new glm::vec3 [nb_vertices];
	glm::vec3 *_vertices = (glm::vec3 *)vertices->data();
	glm::vec2 *_uv = (glm::vec2 *)uvs->data();

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
		GLint *_index = (GLint *)index->data();
		int k0, k1, k2;
		for (int i = 0; i < index->count(); i += 3) {
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
	if (m_pBoundingsphere != nullptr) {
		delete m_pBoundingsphere;
	}

	if (index != nullptr) {
		delete index;
	}

	for (auto attribute: m_attributes) {
		delete attribute.second;
	}
}