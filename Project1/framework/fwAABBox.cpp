#include "fwAABBox.h"

#include <algorithm>    // std::min
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec4.hpp>

#include "../glad/glad.h"

#include "math/fwSphere.h"
#include "math/fwCylinder.h"

#include "fwGeometry.h"
#include "fwMaterialBasic.h"
#include "fwMesh.h"

fwAABBox::fwAABBox()
{
}

fwAABBox::fwAABBox(float x, float x1, float y, float y1, float z, float z1) :
	m_p(x, y, z),
	m_p1(x1, y1, z1)
{
}

fwAABBox::fwAABBox(fwSphere& sphere)
{
	glm::vec3 center = sphere.center();
	float radius = sphere.radius();

	m_p = glm::vec3(center.x - radius, center.y - radius, center.z - radius);
	m_p1 = glm::vec3(center.x + radius, center.y + radius, center.z + radius);
}

fwAABBox::fwAABBox(glm::vec3& p1, glm::vec3& p2)
{
	m_p = glm::min(p1, p2);
	m_p1 = glm::max(p1, p2);
}

fwAABBox::fwAABBox(fwAABBox& source, glm::mat4& matrix)
{
	m_p = glm::vec3(matrix * glm::vec4(source.m_p, 1.0));
	m_p1 = glm::vec3(matrix * glm::vec4(source.m_p1, 1.0));
}

/**
 * Create from a cylinder
 */
fwAABBox::fwAABBox(fwCylinder& cylinder)
{
	float radius = cylinder.radius();
	float height = cylinder.height();
	const glm::vec3& center = cylinder.position();
	m_p = glm::vec3(center.x - radius, center.y, center.z - radius);
	m_p1 = glm::vec3(center.x + radius, center.y + height, center.z + radius);
}

/**
 * change the content
 */
void fwAABBox::set(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax)
{
	m_p.x = xmin;	m_p.y = ymin;	m_p.z = zmin;
	m_p1.x = xmax;	m_p1.y = ymax;	m_p1.z = zmax;
}

/**
 * update based on translation on the source
 */
void fwAABBox::translateFrom(const fwAABBox& source, glm::vec3& translation)
{
	m_p = source.m_p + translation;
	m_p1 = source.m_p1 + translation;
}

/**
 * update based on rotation on the source
 */
void fwAABBox::rotateFrom(fwAABBox& source, glm::vec3& rotation)
{
	m_p = source.m_p + rotation;
	m_p1 = source.m_p1 + rotation;
}

/**
 * apply a matrix4 to a source
 */
void fwAABBox::apply(fwAABBox& source, glm::mat4& matrix)
{
	glm::vec3 p = glm::vec3(matrix * glm::vec4(source.m_p, 1.0));
	glm::vec3 p1 = glm::vec3(matrix * glm::vec4(source.m_p1, 1.0));

	// ensure p is always min and p1 is always max
	m_p.x = std::min(p.x, p1.x);
	m_p.y = std::min(p.y, p1.y);
	m_p.z = std::min(p.z, p1.z);

	m_p1.x = std::max(p.x, p1.x);
	m_p1.y = std::max(p.y, p1.y);
	m_p1.z = std::max(p.z, p1.z);
}

/**
 * check if the AABB was initialized
 */
bool fwAABBox::not_init(void)
{
	return m_p.x == 999999;
}

/**
 * copy a AABB into the other
 */
fwAABBox& fwAABBox::copy(fwAABBox& source)
{
	m_p = source.m_p;
	m_p1 = source.m_p1;

	return *this;
}

fwAABBox& fwAABBox::multiplyBy(float v)
{
	m_p *= v;
	m_p1 *= v;

	return *this;
}

/**
 * If point is inside the AABB
 */
bool fwAABBox::inside(glm::vec3& position)
{
	return (position.x >= m_p.x && position.x <= m_p1.x &&
		position.y >= m_p.y && position.y <= m_p1.y &&
		position.z >= m_p.z && position.z <= m_p1.z);
}

/**
 * If the current AABB is completely inside the given AABB
 */
bool fwAABBox::inside(fwAABBox& box)
{
	// using 6 splitting planes to rule out intersections.
	return (m_p.x >= box.m_p.x && m_p1.x < box.m_p1.x &&
		m_p.y >= box.m_p.y && m_p1.y <= box.m_p1.y &&
		m_p.z >= box.m_p.z && m_p1.z <= box.m_p1.z);
}

/**
 * test if the 2 boxes intesect
 */
bool fwAABBox::intersect(fwAABBox& box)
{
	// using 6 splitting planes to rule out intersections.
	return (box.m_p1.x < m_p.x || box.m_p.x > m_p1.x ||
		box.m_p1.y < m_p.y || box.m_p.y > m_p1.y ||
		box.m_p1.z < m_p.z || box.m_p.z > m_p1.z) ? false : true;
}

/**
 * Extend the current AABBB y including the give one
 */
void fwAABBox::extend(fwAABBox& box)
{
	if (m_p.x > box.m_p.x) m_p.x = box.m_p.x;
	if (m_p1.x < box.m_p1.x) m_p1.x = box.m_p1.x;
	if (m_p.y > box.m_p.y) m_p.y = box.m_p.y;
	if (m_p1.y < box.m_p1.y) m_p1.y = box.m_p1.y;
	if (m_p.z > box.m_p.z) m_p.z = box.m_p.z;
	if (m_p1.z < box.m_p1.z) m_p1.z = box.m_p1.z;
}

/**
 * Extend the current AABBB y including the give point
 */
void fwAABBox::extend(glm::vec3& vertice)
{
	if (m_p.x > vertice.x) m_p.x = vertice.x;
	if (m_p1.x < vertice.x) m_p1.x = vertice.x;
	if (m_p.y > vertice.y) m_p.y = vertice.y;
	if (m_p1.y < vertice.y) m_p1.y = vertice.y;
	if (m_p.z > vertice.z) m_p.z = vertice.z;
	if (m_p1.z < vertice.z) m_p1.z = vertice.z;
}

float fwAABBox::surface(void)
{
	return (m_p1.x - m_p.x) * (m_p1.z - m_p.z);
}

float fwAABBox::volume(void)
{
	return (m_p1.x - m_p.x) * (m_p1.y - m_p.y) * (m_p1.z - m_p.z);
}

/**
 * Set all values back to NONE
 */
void fwAABBox::reset(void)
{
	m_p.x = m_p.y = m_p.z = 999999;
	m_p1.x = m_p1.y = m_p1.z = -99999;
}

/**
 * Translate AABB by the vector
 */
fwAABBox fwAABBox::operator+(const glm::vec3& v)
{
	glm::vec3 p = m_p + v;
	glm::vec3 p1 = m_p1 + v;
	return fwAABBox(p, p1);
}

/**
 * https://en.wikibooks.org/wiki/OpenGL_Programming/Bounding_box
 */
fwMesh *fwAABBox::draw(void)
{
	float _vertices[] = {
		// back face
		m_p.x, m_p.y, m_p.z, // bottom-left
		m_p.x, m_p1.y, m_p.z, // top-left
		m_p1.x, m_p1.y, m_p.z, // top-right
		m_p1.x, m_p.y, m_p.z, // bottom-right         
		// front face
		m_p.x, m_p.y, m_p1.z, // bottom-left
		m_p.x, m_p1.y,  m_p1.z, // top-left
		m_p1.x, m_p1.y, m_p1.z, // top-right
		m_p1.x, m_p.y,m_p1.z, // bottom-right
		// left face
		m_p.x, m_p1.y, m_p1.z, // top-right
		m_p.x, m_p1.y, m_p.z, // top-left
		m_p.x, m_p.y, m_p.z, // bottom-left
		m_p.x, m_p.y, m_p1.z, // bottom-right
		// right face
		m_p1.x, m_p1.y, m_p1.z, // top-left
		m_p.x, m_p1.y, m_p1.z, // top-right         
		m_p1.x, m_p.y, m_p1.z, // bottom-left     
		m_p1.x, m_p.y, m_p.z, // bottom-right
		// bottom face
		m_p.x, m_p.y, m_p.z, // top-right
		m_p1.x, m_p.y, m_p.z, // top-left
		m_p1.x, m_p.y, m_p1.z, // bottom-left
		m_p.x, m_p.y, m_p1.z, // bottom-right
		// top face
		m_p1.x,m_p1.y, m_p1.z, // bottom-right
		m_p.x, m_p1.y, m_p1.z, // bottom-left
		m_p.x, m_p1.y, m_p.z, // top-left
		m_p1.x,m_p1.y, m_p.z, // top-right     
		m_p1.x,m_p1.y, m_p1.z, // bottom-right
		m_p1.x,m_p1.y, m_p.z // top-right     
	};

	fwGeometry* geometry = new fwGeometry();
	float* vertices = (float*)malloc(sizeof(_vertices));

	memcpy(vertices, _vertices, sizeof(_vertices));

	geometry->addVertices("aPos", vertices, 3, sizeof(_vertices), ARRAY_SIZE_OF_ELEMENT(_vertices));

	// shared geometry
	static glm::vec4 w(1.0, 1.0, 1.0, 1.0);
	static fwMaterialBasic white(&w);
	m_mesh = new fwMesh(geometry, &white);
	m_mesh->rendering(fwMeshRendering::FW_MESH_LINES);

	return m_mesh;
}

fwAABBox::~fwAABBox()
{
}
