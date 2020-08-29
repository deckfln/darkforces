#include "fwCollision.h"

#include <glm/glm.hpp>

fwCollision::fwCollision()
{
}

fwCollision::~fwCollision(void)
{
}

// Christer_Ericson-Real-Time_Collision_Detectio
// Given segment pq and triangle abc, returns whether segment intersects
// triangle and if so, also returns the barycentric coordinates (u,v,w)
// of the intersection point
bool Framework::IntersectSegmentTriangle(const glm::vec3& p,
	const glm::vec3& q, 
	const glm::vec3& a,
	const glm::vec3& b, 
	const glm::vec3& c,
	float &u, 
	float &v, 
	float &w, 
	float &t)
{
	glm::vec3 ab=b-a;
	glm::vec3 ac=c-a;
	glm::vec3 qp=p-q;
	
	// Compute triangle normal. Can be precalculated or cached if
	// intersecting multiple segments against the same triangle
	glm::vec3 n = glm::cross(ab, ac);
	
	// Compute denominator d. If d <= 0, segment is parallel to or points
	// away from triangle, so exit early
	float d = glm::dot(qp, n);
	if (d <= 0.0f) 
		return false;
	
	// Compute intersection t value of pq with plane of triangle. A ray
	// intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
	// dividing by d until intersection has been found to pierce triangle
	glm::vec3 ap = p - a; 
	t = glm::dot(ap, n); 
	if (t < 0.0f) 
		return false; 
	
	if (t > d) 
		return false;
	
	// For segment; exclude this code line for a ray test
	// Compute barycentric coordinate components and test if within bounds
	glm::vec3 e = glm::cross(qp, ap);
	v = glm::dot(ac, e);
	if (v < 0.0f || v>d)
		return false;
	
	w = -glm::dot(ab, e);
	if (w < 0.0f || v+w>d)
		return false;
	
	// Segment/ray intersects triangle. Perform delayed division and
	// compute the last barycentric coordinate component
	float ood = 1.0f / d;
	t *= ood;
	v *= ood;
	w *= ood;
	u = 1.0f-v-w;
	return true;
}

// https://www.thetopsites.net/article/53962225.shtml
#define EPSILON 0.000001f
bool Framework::lineSegIntersectTri(
	const glm::vec3 &p,
	const glm::vec3 &q,
	const glm::vec3& a,
	const glm::vec3& b,
	const glm::vec3& c,
	glm::vec3  &point
) {
	glm::vec3 e0 = b - a;
	glm::vec3 e1 = c - a;

	glm::vec3 dir = q - p;
	glm::vec3 dir_norm = glm::normalize(dir);

	glm::vec3 h = glm::cross(dir_norm, e1);
	const float d = glm::dot(e0, h);

	if (d > -EPSILON && d < EPSILON) {
		return false;
	}

	glm::vec3 s = p - a;
	const float f = 1.0f / d;
	const float u = f * glm::dot(s, h);

	if (u < 0.0f || u > 1.0f) {
		return false;
	}

	glm::vec3 q1 = glm::cross(s, e0);
	const float v = f * glm::dot(dir_norm, q1);

	if (v < 0.0f || u + v > 1.0f) {
		return false;
	}

	const float t = f * glm::dot(e1, q1);

	if (t > EPSILON && t < sqrtf(glm::dot(dir, dir))) { // segment intersection
		point = p + dir_norm * t;

		return true;
	}

	return false;
}

/**
 * Sphere/Triangle intersection (other version)
 */
bool Framework::intersectSphereTriangle(
	const glm::vec3& center_es, 
	const glm::vec3& a,	const glm::vec3 b, const glm::vec3 c, 
	glm::vec3& p)
{
	// plane equation
	glm::vec3 normal;
	glm::vec4 plane;
	float signedDistance;

	// triangle equation
	glm::vec3 tr0;
	glm::vec3 tr1;
	float tarea, tarea1, tarea2, tarea3;

	// first, test the intersection with the triangle plane
	normal = glm::normalize(glm::cross(b - a, c - a));
	plane = glm::vec4(normal.x, normal.y, normal.z, -(normal.x * a.x + normal.y * a.y + normal.z * a.z));
	signedDistance = glm::dot(center_es, normal) + plane.w;

	if (signedDistance >= -1.0f && signedDistance <= 1.0f) {
		// if the plane is passing trough the sphere (the ellipsoid deformed to look like a sphere)
		// get the collision point of the sphere on the plane

		p = center_es - normal * glm::abs(signedDistance);

		// test if the collision origin in INSIDE the triangle

		// area of the GL triangle
		tr0 = a - b;
		tr1 = c - b;
		tarea = glm::length(glm::cross(tr0, tr1));

		// area of the triangles using the intersection point as vertex
		tr0 = a - p;
		tr1 = b - p;
		tarea1 = glm::length(glm::cross(tr0, tr1));

		tr0 = b - p;
		tr1 = c - p;
		tarea2 = glm::length(glm::cross(tr0, tr1));

		tr0 = c - p;
		tr1 = a - p;
		tarea3 = glm::length(glm::cross(tr0, tr1));

		// if the sum of the 3 new triangles is equal to the opengl triangle
		if (abs(tarea1 + tarea2 + tarea3 - tarea) < 0.01) {
			return true;
		}
	}

	return false;
}

/********************************************************
 * https://gamedev.stackexchange.com/questions/65723/finding-z-given-x-y-coordinates-on-terrain
 */
template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

static bool same_sign(const float a, const float b) {
	return sgn(a) == sgn(b);
}

static float scalar_product(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
	return glm::dot(glm::cross(a, b), c);
}

bool Framework::IntersectLineTriangle(
	const glm::vec3& p, const glm::vec3& q, 
	const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
	glm::vec3& collision)
{
	const glm::vec3 pq = q - p;
	const glm::vec3 pa = a - p;
	const glm::vec3 pb = b - p;
	const glm::vec3 pc = c - p;

	glm::vec3 m = glm::cross(pq, pc);
	double u = glm::dot(pb, m);
	double v = -glm::dot(pa, m);
	if (!same_sign(u, v)) return false;
	double w = scalar_product(pq, pb, pa);
	if (!same_sign(u, w)) return false;

	if (u + v + w == 0) return false;

	const double denom = 1.0 / (u + v + w);
	u *= denom;
	v *= denom;
	w *= denom; // w = 1.0f - u - v;

	collision.x = (u * a.x + v * b.x + w * c.x);
	collision.y = (u * a.y + v * b.y + w * c.y);
	collision.z = (u * a.z + v * b.z + w * c.z);

	return true;
}
