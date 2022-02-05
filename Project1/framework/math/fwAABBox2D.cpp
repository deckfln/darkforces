#include "fwAABBox2D.h"

#include <algorithm>

Framework::AABBox2D::AABBox2D():
	m_p(+INFINITY, +INFINITY),
	m_p1(-INFINITY, -INFINITY)
{
}

Framework::AABBox2D::AABBox2D(const glm::vec2& p, const glm::vec2& p1)
{
	m_p.x = std::min(p.x, p1.x);
	m_p.y = std::min(p.y, p1.y);

	m_p1.x = std::max(p.x, p1.x);
	m_p1.y = std::max(p.y, p1.y);
}

Framework::AABBox2D::AABBox2D(float x, float y, float x1, float y1)
{
	m_p.x = std::min(x, x1);
	m_p.y = std::min(y, y1);

	m_p1.x = std::max(x, x1);
	m_p1.y = std::max(y, y1);
}

Framework::AABBox2D::AABBox2D(const Framework::Segment2D& segment)
{
	const glm::vec2& p = segment.start();
	const glm::vec2& p1 = segment.end();

	m_p.x = std::min(p.x, p1.x);
	m_p.y = std::min(p.y, p1.y);

	m_p1.x = std::max(p.x, p1.x);
	m_p1.y = std::max(p.y, p1.y);
}

void Framework::AABBox2D::set(float x, float y, float x1, float y1)
{
	m_p.x = std::min(x, x1);
	m_p.y = std::min(y, y1);

	m_p1.x = std::max(x, x1);
	m_p1.y = std::max(y, y1);
}

void Framework::AABBox2D::set(const glm::vec2& p, const glm::vec2& p1)
{
	m_p.x = std::min(p.x, p1.x);
	m_p.y = std::min(p.y, p1.y);

	m_p1.x = std::max(p.x, p1.x);
	m_p1.y = std::max(p.y, p1.y);
}

void Framework::AABBox2D::set(const glm::vec2* v, uint32_t nb)
{
	for (uint32_t i = 0; i < nb; i++) {
		if (v[i].x < m_p.x) {	m_p.x = v[i].x;	}
		if (v[i].y < m_p.y) {	m_p.y = v[i].y;	}
		if (v[i].x > m_p1.x) {	m_p1.x = v[i].x;}
		if (v[i].y > m_p1.y) {	m_p1.y = v[i].y;}
	}
}

constexpr float MY_EPSILON = 2 * FLT_EPSILON;

/**
 * intersect with a segment (ray like)
 */
bool Framework::AABBox2D::alignedPlan(float t, const Framework::Segment2D& segment, float& t1, glm::vec2& p)
{
	float x, y;

	if (t < 0.0f || t > 1.0f) {
		// the collision is BEFORE or AFTER the 2 end of the segment
		return false;
	}

	x = (segment.end().x - segment.start().x) * t + segment.start().x;
	y = (segment.end().y - segment.start().y) * t + segment.start().y;

	// check if (x,y,z) is on the AABB panel
	if (x >= (m_p.x - MY_EPSILON) && x <= (m_p1.x + MY_EPSILON) &&
		y >= (m_p.y - MY_EPSILON) && y <= (m_p1.y + MY_EPSILON)) {
		if (t < t1) {
			p.x = x;
			p.y = y;
			t1 = t;
			return true;
		}
	}
	return false;
}

bool Framework::AABBox2D::xAlignedPlan(float x, const Framework::Segment2D& segment, float& t1, glm::vec2& p)
{
	// test AxisAligned plan on m_p.x
	// t = (x - x0) / (x1 - x0)
	float t;
	float dx = segment.end().x - segment.start().x;

	if (dx == 0) {
		return false;
	}

	t = (x - segment.start().x) / dx;
	return alignedPlan(t, segment, t1, p);
}

bool Framework::AABBox2D::yAlignedPlan(float y, const Framework::Segment2D& segment, float& t1, glm::vec2& p)
{
	float t;
	float dy = segment.end().y - segment.start().y;
	if (dy == 0) {
		return false;
	}
	t = (y - segment.start().y) / dy;
	return alignedPlan(t, segment, t1, p);
}

bool Framework::AABBox2D::intersect(const Framework::Segment2D& segment, glm::vec2& p)
{
	AABBox2D aabb(segment);
	if (!intersect(aabb)) {
		return false;
	}

	// if both points of the segment are inside the AABB, return the center of the AABB
	/*
	if (inside(segment.m_start) && inside(segment.m_end)) {
		p = segment.m_end;
		return Intersection::INCLUDED;
	}
	*/

	float t = +INFINITY;

	// equation of the line
	// P = (p1 - p0) * t + p0
	//x = (x1 - x0) * t + x0
	//y = (y1 - y0) * t + y0
	//z = (z1 - z0) * t + z0
	xAlignedPlan(m_p.x, segment, t, p);
	xAlignedPlan(m_p1.x, segment, t, p);
	yAlignedPlan(m_p.y, segment, t, p);
	yAlignedPlan(m_p1.y, segment, t, p);

	return (t < +INFINITY) ? true : false;
}

/**
 * intersect with a box
 */
bool Framework::AABBox2D::intersect(const AABBox2D& box)
{
	// using 6 splitting planes to rule out intersections.
	return (box.m_p1.x < (m_p.x - MY_EPSILON) || box.m_p.x >(m_p1.x + MY_EPSILON) ||
		box.m_p1.y < (m_p.y - MY_EPSILON) || box.m_p.y >(m_p1.y + MY_EPSILON)) ? false : true;
}

/**
 * If point is inside the AABB
 */
bool Framework::AABBox2D::inside(const glm::vec2& p)
{
	return (p.x >= (m_p.x - MY_EPSILON) && p.x <= (m_p1.x + MY_EPSILON) &&
			p.y >= (m_p.y - MY_EPSILON) && p.y <= (m_p1.y + MY_EPSILON));
}

/**
 * extend the AABB using the point
 */
void Framework::AABBox2D::extend(const glm::vec2& p)
{
	if (p.x < m_p.x) { m_p.x = p.x; };
	if (p.x > m_p1.x) { m_p1.x = p.x; };
	if (p.y < m_p.y) { m_p.y = p.y; };
	if (p.y > m_p1.y) { m_p1.y = p.y; };
}
