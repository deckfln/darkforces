#include "fwAABBox.h"

#include <vector>
#include <algorithm>    // std::min
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec4.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../glad/glad.h"

#include "../glEngine/glBufferAttribute.h"

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

fwAABBox::fwAABBox(const glm::vec3& p1, const glm::vec3& p2)
{
	set(p1, p2);
}

fwAABBox::fwAABBox(const fwAABBox& source, const glm::mat4& matrix)
{
	apply(source, matrix);
}

fwAABBox::fwAABBox(const fwAABBox* source, const glm::mat4& matrix)
{
	apply(source, matrix);
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
 * create from a segment
 */
fwAABBox::fwAABBox(const Framework::Segment& segment)
{
	set(segment.m_start, segment.m_end);
}

/**
 * change the content
 */
void fwAABBox::set(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax)
{
	m_p.x = xmin;	m_p.y = ymin;	m_p.z = zmin;
	m_p1.x = xmax;	m_p1.y = ymax;	m_p1.z = zmax;
	m_dirty = true;
}

/**
 * Set from vertices
 */
void fwAABBox::set(std::vector<glm::vec3>& vertices)
{
	m_p = glm::vec3(99999, 99999, 99999);
	m_p1 = glm::vec3(-99999, -99999, -99999);

	for (glm::vec3& vertice : vertices) {
		extend(vertice);
	}
	m_dirty = true;
}

/**
 * build from an GL attribute
 */
void fwAABBox::set(glBufferAttribute* attribute)
{
	float minX = INFINITY;
	float minY = INFINITY;
	float minZ = INFINITY;

	float maxX = -INFINITY;
	float maxY = -INFINITY;
	float maxZ = -INFINITY;

	for (unsigned int i = 0; i < attribute->count(); ++i) {
		glm::vec3* v = (glm::vec3*)attribute->get_index(i);

		if (v->x < minX) minX = v->x;
		if (v->y < minY) minY = v->y;
		if (v->z < minZ) minZ = v->z;

		if (v->x > maxX) maxX = v->x;
		if (v->y > maxY) maxY = v->y;
		if (v->z > maxZ) maxZ = v->z;
	}
	m_p = glm::vec3(minX, minY, minZ);
	m_p1= glm::vec3(maxX, maxY, maxZ);

	m_dirty = true;
}

/**
 * build from vertices pointers
 */
void fwAABBox::set(glm::vec3 const* pVertices, int nb)
{
	float minX = INFINITY;
	float minY = INFINITY;
	float minZ = INFINITY;

	float maxX = -INFINITY;
	float maxY = -INFINITY;
	float maxZ = -INFINITY;

	for (unsigned int i = 0; i < nb; i++) {
		if (pVertices[i].x < minX) minX = pVertices[i].x;
		if (pVertices[i].y < minY) minY = pVertices[i].y;
		if (pVertices[i].z < minZ) minZ = pVertices[i].z;

		if (pVertices[i].x > maxX) maxX = pVertices[i].x;
		if (pVertices[i].y > maxY) maxY = pVertices[i].y;
		if (pVertices[i].z > maxZ) maxZ = pVertices[i].z;
	}
	m_p = glm::vec3(minX, minY, minZ);
	m_p1 = glm::vec3(maxX, maxY, maxZ);

	m_dirty = true;
}

/**
 * build from 2 points
 */
void fwAABBox::set(const glm::vec3& p1, const glm::vec3& p2)
{
	m_p = glm::min(p1, p2);
	m_p1 = glm::max(p1, p2);
	m_dirty = true;
}

/**
 * update based on translation on the source
 */
void fwAABBox::translateFrom(const fwAABBox& source, glm::vec3& translation)
{
	m_p = source.m_p + translation;
	m_p1 = source.m_p1 + translation;
	m_dirty = true;
}

/**
 * update based on rotation on the source
 */
void fwAABBox::rotateFrom(const fwAABBox& source, const glm::vec3& rotation)
{
	glm::quat quaternion = glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));
	rotateFrom(source, quaternion);
	m_dirty = true;
}

/**
 * update based on rotation on the source
 */
void fwAABBox::rotateFrom(const fwAABBox& source, const glm::quat& quaternion)
{
	glm::mat4 rotationMatrix = glm::toMat4(quaternion);

	// rebuild the 8 vertices
	glm::vec3 delta = source.m_p1 - source.m_p;

	std::vector<glm::vec3> points = {
		rotationMatrix * glm::vec4(source.m_p, 1.0),
		rotationMatrix * glm::vec4(source.m_p + glm::vec3(delta.x, 0, 0), 1.0),
		rotationMatrix * glm::vec4(source.m_p + glm::vec3(delta.x, delta.y, 0), 1.0),
		rotationMatrix * glm::vec4(source.m_p + glm::vec3(0, delta.y, 0), 1.0),
		rotationMatrix * glm::vec4(source.m_p + glm::vec3(0, 0, delta.z), 1.0),
		rotationMatrix * glm::vec4(source.m_p + glm::vec3(delta.x, 0, delta.z), 1.0),
		rotationMatrix * glm::vec4(source.m_p + glm::vec3(delta.x, delta.y, delta.z), 1.0),
		rotationMatrix * glm::vec4(source.m_p + glm::vec3(0, delta.y, delta.z), 1.0)
	};

	// and extract the min and max
	set(points);

	m_dirty = true;
}

/**
 * update based on translation & rotation on the source
 */
void fwAABBox::transform(const fwAABBox& source, glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
{
	glm::quat quaternion = glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));
	glm::mat4 rotationMatrix = glm::toMat4(quaternion);

	glm::mat4 translateMatrix = glm::translate(translation);
	glm::mat4 scaleMatrix = glm::scale(scale);

	// rebuild the 8 vertices
	glm::vec3 delta = source.m_p1 - source.m_p;

	std::vector<glm::vec3> points = {
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p, 1.0),
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p + glm::vec3(delta.x, 0, 0), 1.0),
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p + glm::vec3(delta.x, delta.y, 0), 1.0),
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p + glm::vec3(0, delta.y, 0), 1.0),
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p + glm::vec3(0, 0, delta.z), 1.0),
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p + glm::vec3(delta.x, 0, delta.z), 1.0),
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p + glm::vec3(delta.x, delta.y, delta.z), 1.0),
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p + glm::vec3(0, delta.y, delta.z), 1.0)
	};

	// and extract the min and max
	set(points);

	m_dirty = true;
}

void fwAABBox::transform(const fwAABBox& source, const glm::vec3& translation, const glm::quat& quaternion, const glm::vec3& scale)
{
	glm::mat4 rotationMatrix = glm::toMat4(quaternion);

	glm::mat4 translateMatrix = glm::translate(translation);
	glm::mat4 scaleMatrix = glm::scale(scale);

	// rebuild the 8 vertices
	glm::vec3 delta = source.m_p1 - source.m_p;

	std::vector<glm::vec3> points = {
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p, 1.0),
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p + glm::vec3(delta.x, 0, 0), 1.0),
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p + glm::vec3(delta.x, delta.y, 0), 1.0),
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p + glm::vec3(0, delta.y, 0), 1.0),
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p + glm::vec3(0, 0, delta.z), 1.0),
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p + glm::vec3(delta.x, 0, delta.z), 1.0),
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p + glm::vec3(delta.x, delta.y, delta.z), 1.0),
		translateMatrix * scaleMatrix * rotationMatrix * glm::vec4(source.m_p + glm::vec3(0, delta.y, delta.z), 1.0)
	};

	// and extract the min and max
	set(points);

	m_dirty = true;
}

void fwAABBox::transform(const glm::mat4& matrix)
{
	// rebuild the 8 vertices
	glm::vec3 delta = m_p1 - m_p;

	std::vector<glm::vec3> points = {
		matrix * glm::vec4(m_p,									 1.0),
		matrix * glm::vec4(m_p + glm::vec3(delta.x, 0,		 0), 1.0),
		matrix * glm::vec4(m_p + glm::vec3(delta.x, delta.y, 0), 1.0),
		matrix * glm::vec4(m_p + glm::vec3(0,		delta.y, 0), 1.0),
		matrix * glm::vec4(m_p + glm::vec3(0,		0,		 delta.z), 1.0),
		matrix * glm::vec4(m_p + glm::vec3(delta.x, 0,		 delta.z), 1.0),
		matrix * glm::vec4(m_p + glm::vec3(delta.x, delta.y, delta.z), 1.0),
		matrix * glm::vec4(m_p + glm::vec3(0,		delta.y, delta.z), 1.0)
	};

	// and extract the min and max
	set(points);
	m_dirty = true;
}

/**
 * apply a matrix4 to a source
 */
void fwAABBox::apply(const fwAABBox& source, const glm::mat4& matrix)
{
	// rebuild the 8 vertices
	glm::vec3 delta = source.m_p1 - source.m_p;

	std::vector<glm::vec3> points = {
		matrix * glm::vec4(source.m_p, 1.0),
		matrix * glm::vec4(source.m_p + glm::vec3(delta.x, 0, 0), 1.0),
		matrix * glm::vec4(source.m_p + glm::vec3(delta.x, delta.y, 0), 1.0),
		matrix * glm::vec4(source.m_p + glm::vec3(0, delta.y, 0), 1.0),
		matrix * glm::vec4(source.m_p + glm::vec3(0, 0, delta.z), 1.0),
		matrix * glm::vec4(source.m_p + glm::vec3(delta.x, 0, delta.z), 1.0),
		matrix * glm::vec4(source.m_p + glm::vec3(delta.x, delta.y, delta.z), 1.0),
		matrix * glm::vec4(source.m_p + glm::vec3(0, delta.y, delta.z), 1.0)
	};

	// and extract the min and max
	set(points);
	m_dirty = true;
}

void fwAABBox::apply(fwAABBox const *pSource, const glm::mat4& matrix)
{
	// rebuild the 8 vertices
	glm::vec3 delta = pSource->m_p1 - pSource->m_p;

	std::vector<glm::vec3> points = {
		matrix * glm::vec4(pSource->m_p, 1.0),
		matrix * glm::vec4(pSource->m_p + glm::vec3(delta.x, 0, 0), 1.0),
		matrix * glm::vec4(pSource->m_p + glm::vec3(delta.x, delta.y, 0), 1.0),
		matrix * glm::vec4(pSource->m_p + glm::vec3(0, delta.y, 0), 1.0),
		matrix * glm::vec4(pSource->m_p + glm::vec3(0, 0, delta.z), 1.0),
		matrix * glm::vec4(pSource->m_p + glm::vec3(delta.x, 0, delta.z), 1.0),
		matrix * glm::vec4(pSource->m_p + glm::vec3(delta.x, delta.y, delta.z), 1.0),
		matrix * glm::vec4(pSource->m_p + glm::vec3(0, delta.y, delta.z), 1.0)
	};

	// and extract the min and max
	set(points);
	m_dirty = true;
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
	m_dirty = true;

	return *this;
}

fwAABBox& fwAABBox::multiplyBy(float v)
{
	m_p *= v;
	m_p1 *= v;
	m_dirty = true;

	return *this;
}

/**
 * If point is inside the AABB
 */
bool fwAABBox::inside(const glm::vec3& position)
{
	return (position.x >= (m_p.x - FLT_EPSILON) && position.x <= (m_p1.x + FLT_EPSILON) &&
		position.y >= (m_p.y - FLT_EPSILON) && position.y <= (m_p1.y + FLT_EPSILON) &&
		position.z >= (m_p.z - FLT_EPSILON) && position.z <= (m_p1.z + FLT_EPSILON));
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
 * test if the 2 boxes intersect
 */
bool fwAABBox::intersect(const fwAABBox& box)
{
	// using 6 splitting planes to rule out intersections.
	return (box.m_p1.x < m_p.x || box.m_p.x > m_p1.x ||
		box.m_p1.y < m_p.y || box.m_p.y > m_p1.y ||
		box.m_p1.z < m_p.z || box.m_p.z > m_p1.z) ? false : true;
}

bool fwAABBox::intersect(fwAABBox* box)
{
	// using 6 splitting planes to rule out intersections.
	return (box->m_p1.x < m_p.x || box->m_p.x > m_p1.x ||
		box->m_p1.y < m_p.y || box->m_p.y > m_p1.y ||
		box->m_p1.z < m_p.z || box->m_p.z > m_p1.z) ? false : true;
}

/**
 * intersect with a segment (ray like)
 */
bool fwAABBox::alignedPlan(float t, const Framework::Segment& segment, float& t1, glm::vec3& p)
{
	float x, y, z;

	if (t < 0.0f || t > 1.0f) {
		// the collision is BEFORE or AFTER the 2 end of the segment
		return false;
	}

	x = (segment.m_end.x - segment.m_start.x) * t + segment.m_start.x;
	y = (segment.m_end.y - segment.m_start.y) * t + segment.m_start.y;
	z = (segment.m_end.z - segment.m_start.z) * t + segment.m_start.z;

	// check if (x,y,z) is on the AABB panel
	if (x >= (m_p.x - FLT_EPSILON) && x <= (m_p1.x + FLT_EPSILON) && 
		y >= (m_p.y - FLT_EPSILON) && y <= (m_p1.y + FLT_EPSILON) &&
		z >= (m_p.z - FLT_EPSILON) && z <= (m_p1.z + FLT_EPSILON)) {
		if (t < t1) {
			p.x = x;
			p.y = y;
			p.z = z;
			t1 = t;
			return true;
		}
	}
	return false;
}

bool fwAABBox::xAlignedPlan(float x, const Framework::Segment& segment, float& t1, glm::vec3& p)
{
	// test AxisAligned plan on m_p.x
	// t = (x - x0) / (x1 - x0)
	float t;
	float dx = segment.m_end.x - segment.m_start.x;

	if (dx == 0) {
		return false;
	}

	t = (x - segment.m_start.x) / dx;
	return alignedPlan(t, segment, t1, p);
}

bool fwAABBox::yAlignedPlan(float y, const Framework::Segment& segment, float& t1, glm::vec3& p)
{
	float t;
	float dy = segment.m_end.y - segment.m_start.y;
	if (dy == 0) {
		return false;
	}
	t = (y - segment.m_start.y) / dy;
	return alignedPlan(t, segment, t1, p);
}

bool fwAABBox::zAlignedPlan(float z, const Framework::Segment& segment, float& t1, glm::vec3& p)
{
	float t;
	float dz = segment.m_end.z - segment.m_start.z;
	if (dz == 0) {
		return false;
	}
	t = (z - segment.m_start.z) / dz;
	return alignedPlan(t, segment, t1, p);
}

bool fwAABBox::intersect(const Framework::Segment& segment, glm::vec3& p)
{
	fwAABBox aabb(segment);
	if (!intersect(aabb)) {
		return false;
	}

	// if both points of the segment are inside the AABB, return the center of the AABB
	if (inside(segment.m_start) && inside(segment.m_end)) {
		p = segment.m_end;
		return true;
	}

	float t = +INFINITY;

	// equation of the line
	// P = (p1 - p0) * t + p0
	//x = (x1 - x0) * t + x0
	//y = (y1 - y0) * t + y0
	//z = (z1 - z0) * t + z0
	xAlignedPlan(m_p.x,  segment, t, p);
	xAlignedPlan(m_p1.x, segment, t, p);
	yAlignedPlan(m_p.y,  segment, t, p);
	yAlignedPlan(m_p1.y, segment, t, p);
	zAlignedPlan(m_p.z,  segment, t, p);
	zAlignedPlan(m_p1.z, segment, t, p);

	return t < +INFINITY;
}

/**
 * intersect with a ray
 * https://github.com/erich666/GraphicsGems/blob/master/gems/RayBox.c
 */
enum class RAY_INTERSECT
{
	RIGHT,
	LEFT,
	MIDDLE
};
#define NUMDIM 3

bool fwAABBox::intersect(const glm::vec3& ray_orig, const glm::vec3& ray_dir, glm::vec3& point)
{
	bool inside = TRUE;
	RAY_INTERSECT quadrant[NUMDIM];
	register int i;
	int whichPlane;
	float maxT[NUMDIM];
	float candidatePlane[NUMDIM];

	float minB[NUMDIM] = { m_p.x, m_p.y, m_p.z },
		maxB[NUMDIM] = { m_p1.x, m_p1.y, m_p1.z };		/*box */
	float origin[NUMDIM] = { ray_orig.x, ray_orig.y, ray_orig.z },
		dir[NUMDIM] = { ray_dir.x, ray_dir.y, ray_dir.z };		/*ray */
	float coord[NUMDIM];					/* hit point */

	/* Find candidate planes; this loop can be avoided if
	rays cast all from the eye(assume perpsective view) */
	for (i = 0; i < NUMDIM; i++)
		if (origin[i] < minB[i]) {
			quadrant[i] = RAY_INTERSECT::LEFT;
			candidatePlane[i] = minB[i];
			inside = FALSE;
		}
		else if (origin[i] > maxB[i]) {
			quadrant[i] = RAY_INTERSECT::RIGHT;
			candidatePlane[i] = maxB[i];
			inside = FALSE;
		}
		else {
			quadrant[i] = RAY_INTERSECT::MIDDLE;
		}

	/* Ray origin inside bounding box */
	if (inside) {
		point = ray_orig;
		return true;
	}


	/* Calculate T distances to candidate planes */
	for (i = 0; i < NUMDIM; i++)
		if (quadrant[i] != RAY_INTERSECT::MIDDLE && dir[i] != 0.)
			maxT[i] = (candidatePlane[i] - origin[i]) / dir[i];
		else
			maxT[i] = -1.;

	/* Get largest of the maxT's for final choice of intersection */
	whichPlane = 0;
	for (i = 1; i < NUMDIM; i++)
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;

	/* Check final candidate actually inside box */
	if (maxT[whichPlane] < 0.) return false;
	for (i = 0; i < NUMDIM; i++)
		if (whichPlane != i) {
			coord[i] = origin[i] + maxT[whichPlane] * dir[i];
			if (coord[i] < minB[i] || coord[i] > maxB[i])
				return false;
		}
		else {
			coord[i] = candidatePlane[i];
		}

	point.x = coord[0];
	point.y = coord[1];
	point.z = coord[2];

	return true;				/* ray hits box */
}

/**
 * if the current AABB is above the given one
 */
bool fwAABBox::isAbove(const fwAABBox& box)
{
	return m_p.y > box.m_p.y;
}

/**
 * Extend the current AABBB y including the give one
 */
void fwAABBox::extend(const fwAABBox& box)
{
	if (m_p.x > box.m_p.x) m_p.x = box.m_p.x;
	if (m_p1.x < box.m_p1.x) m_p1.x = box.m_p1.x;
	if (m_p.y > box.m_p.y) m_p.y = box.m_p.y;
	if (m_p1.y < box.m_p1.y) m_p1.y = box.m_p1.y;
	if (m_p.z > box.m_p.z) m_p.z = box.m_p.z;
	if (m_p1.z < box.m_p1.z) m_p1.z = box.m_p1.z;
	m_dirty = true;
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
	m_dirty = true;
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
 * return the center of the box
 */
const glm::vec3 fwAABBox::center(void) const
{
	return (m_p + m_p1) / 2.0f;
}

/**
 * return the height og the AABB
 */
float fwAABBox::height(void)
{
	return m_p1.y - m_p.y;
}

/**
 * Translate AABB by the vector
 */
fwAABBox fwAABBox::operator+(const glm::vec3& v)
{
	glm::vec3 p = m_p + v;
	glm::vec3 p1 = m_p1 + v;
	m_dirty = true;
	return fwAABBox(p, p1);
}
/**
 * Translate AABB by the vector
 */
fwAABBox& fwAABBox::operator+=(const glm::vec3& v)
{
	m_p += v;
	m_p1 += v;

	m_dirty = true;

	return *this;
}

/**
 * vector from current AABB to the other one
 */
glm::vec3 fwAABBox::to(const fwAABBox& to)
{
	glm::vec3 center = (m_p1 + m_p) / 2.0f;
	glm::vec3 center1 = (to.m_p1 + to.m_p) / 2.0f;

	return center1 - center;
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

/**
 * pate the vertices of the boundingbox mesh
 */
bool fwAABBox::updateMeshVertices(glm::vec3* vertices, glm::vec3 *colors)
{
	if (m_dirty) {
		// back face
		vertices[0] = { m_p.x, m_p.y, m_p.z };
		vertices[1] = { m_p.x, m_p1.y, m_p.z }; // top-left
		vertices[2] = { m_p1.x, m_p1.y, m_p.z }; // top-right
		vertices[3] = { m_p1.x, m_p.y, m_p.z }; // bottom-right         
			// front face
		vertices[4] = { m_p.x, m_p.y, m_p1.z }; // bottom-left
		vertices[5] = { m_p.x, m_p1.y, m_p1.z }; // top-left
		vertices[6] = { m_p1.x, m_p1.y, m_p1.z }; // top-right
		vertices[7] = { m_p1.x, m_p.y, m_p1.z }; // bottom-right
			// left face
		vertices[8] = { m_p.x, m_p1.y, m_p1.z }; // top-right
		vertices[9] = { m_p.x, m_p1.y, m_p.z }; // top-left
		vertices[10] = { m_p.x, m_p.y, m_p.z }; // bottom-left
		vertices[11] = { m_p.x, m_p.y, m_p1.z }; // bottom-right
			// right face
		vertices[12] = { m_p1.x, m_p1.y, m_p1.z }; // top-left
		vertices[13] = { m_p.x, m_p1.y, m_p1.z }; // top-right         
		vertices[14] = { m_p1.x, m_p.y, m_p1.z }; // bottom-left     
		vertices[15] = { m_p1.x, m_p.y, m_p.z }; // bottom-right
			// bottom face
		vertices[16] = { m_p.x, m_p.y, m_p.z }; // top-right
		vertices[17] = { m_p1.x, m_p.y, m_p.z }; // top-left
		vertices[18] = { m_p1.x, m_p.y, m_p1.z }; // bottom-left
		vertices[19] = { m_p.x, m_p.y, m_p1.z }; // bottom-right
			// top face
		vertices[20] = { m_p1.x, m_p1.y, m_p1.z }; // bottom-right
		vertices[21] = { m_p.x, m_p1.y, m_p1.z }; // bottom-left
		vertices[22] = { m_p.x, m_p1.y, m_p.z }; // top-left
		vertices[23] = { m_p1.x, m_p1.y, m_p.z }; // top-right     
		vertices[24] = { m_p1.x, m_p1.y, m_p1.z }; // bottom-right
		vertices[25] = { m_p1.x,m_p1.y, m_p.z }; // top-right     

		if (colors) {
			// update the color attribute if it exists
			for (auto i=0; i<26; i++)
				colors[i] = m_color;
		}

		m_dirty = false;

		return true;
	}

	return false;
}

/**
 * If the point align vertical with the AABB (is inside OR on top OR just below)
 * using opengl coordinate
 */
bool fwAABBox::verticalAlign(const glm::vec3& point)
{
	return point.x >= m_p.x && point.x <= m_p1.x && point.z >= m_p.z && point.z <= m_p1.z;
}

/**
 * return the debug color
 */
glm::vec3& fwAABBox::color(void)
{
	return m_color;
}

/**
 * set the debug color
 */
fwAABBox& fwAABBox::color(const glm::vec3& color)
{
	m_color = color;
	m_dirty = true;
	return *this;
}

void fwAABBox::recordState(flightRecorder::AABBox* record)
{
	record->classID = flightRecorder::TYPE::AABBOX;
	record->p = m_p;
	record->p1 = m_p1;
}

void fwAABBox::loadState(flightRecorder::AABBox* record)
{
	m_p = record->p;
	m_p1 = record->p1;
	m_dirty = true;		// force debug AABOX to refresh
}

fwAABBox::~fwAABBox()
{
}
