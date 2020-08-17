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
bool IntersectSegmentTriangle(const glm::vec3& p, 
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
bool lineSegIntersectTri(
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