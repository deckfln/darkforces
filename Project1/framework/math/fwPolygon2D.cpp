#include "fwPolygon2D.h"

#include "fwSegment2D.h"

Framework::Primitive::Polygon2D::Polygon2D(void)
{
}

Framework::Primitive::Polygon2D::Polygon2D(const std::vector<glm::vec2>& points)
{
	m_points.resize(points.size());
	for (size_t i = 0; i < points.size(); i++) {
		m_points[i] = points[i];
		m_aabb.extend(points[i]);
	}
}

Framework::Primitive::Polygon2D::Polygon2D(const glm::vec2* points, uint32_t nb)
{
	m_points.resize(nb);
	for (size_t i = 0; i < nb; i++) {
		m_points[i] = points[i];
		m_aabb.extend(points[i]);
	}
}

/**
 *
 */
void Framework::Primitive::Polygon2D::addPoint(const glm::vec2& p)
{
	m_points.push_back(p);
	m_aabb.extend(p);
}

/**
 * Is a 2D point inside a polygon
 */
bool Framework::Primitive::Polygon2D::isPointInside(const glm::vec2& p)
{
	// quick test
	if (!m_aabb.inside(p)) {
		return false;
	}

	// https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon
	// http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
	bool inside = false;

	// test holes
	for (size_t i = 0; i < m_holes.size(); i++) {
		std::vector<glm::vec2>& line = m_holes[i];

		for (size_t i = 0, j = line.size() - 1; i < line.size(); j = i++)
		{
			if ((line[i].y > p.y) != (line[j].y > p.y) &&
				p.x < (line[j].x - line[i].x) * (p.y - line[i].y) / (line[j].y - line[i].y) + line[i].x)
			{
				inside = !inside;
			}
		}
		if (inside) {
			return false;	// if we are in the hole, we are not on the sector
		}
	}

	// finish with the external line
	for (size_t i = 0, j = m_points.size() - 1; i < m_points.size(); j = i++)
	{
		if ((m_points[i].y > p.y) != (m_points[j].y > p.y) &&
			p.x < (m_points[j].x - m_points[i].x) * (p.y - m_points[i].y) / (m_points[j].y - m_points[i].y) + m_points[i].x)
		{
			inside = !inside;
		}
	}

	return inside;
}

/**
 * if the given segment intersect with any of the outter polyline
 */
bool Framework::Primitive::Polygon2D::intersect(Framework::Segment2D& seg)
{
	Segment2D line;
	glm::vec2 p;

	for (size_t i = 0; i < m_points.size() - 1; i++)
	{
		line.set(m_points[i], m_points[i + 1]);
		if (line.intersect(seg, p)) {
			return true;
		}
	}
	return false;
}

/**
 * fwAABBox2D
 */
bool Framework::Primitive::Polygon2D::AABBcollide(AABBox2D& aabb)
{
	if (m_aabb.inside(aabb)) {
		return true;
	}

	if (!aabb.intersect(m_aabb)) {
		return false;
	}
	/*
	printf("polyline\n");
	for (size_t i = 0; i < m_points.size() - 1; i++) {
		printf("%.2f,%.2f,\n", m_points[i].x, m_points[i].y);
	}
	*/
/*
	printf("box\n");
	printf("%.2f,%.2f,%.2f,%.2f,\n", aabb.min().x, aabb.min().y, aabb.max().x, aabb.max().y);
*/
	bool inside = false;
	const glm::vec2& pmin = aabb.min();
	const glm::vec2& pmax = aabb.max();


	// quicktest2: if any of the 4 flat corners are inside the 2D polygon
	glm::vec2 corner(pmin.x, pmin.y);
	if (isPointInside(corner)) {
		return true;
	}
	else {
		corner.x = pmax.x;
		if (isPointInside(corner)) {
			return true;
		}
		else {
			corner.y = pmax.y;
			if (isPointInside(corner)) {
				return true;
			}
			else {
				corner.x = pmin.x;
				if (isPointInside(corner)) {
					return true;
				}
			}
		}
	}

	// if no corner is inside the 2D poliygon, check if the 2D aabb cross any of the polylines
	if (!inside) {
		glm::vec2 p0(pmin.x, pmin.y);
		glm::vec2 p1(pmax.x, pmin.y);
		glm::vec2 p;

		Segment2D border(p0, p1);

		if (intersect(border)) {
			return true;
		}

		p0 = p1;
		p1.x = pmax.x; p1.y = pmax.y;
		border.set(p0, p1);
		if (intersect(border)) {
			return true;
		}

		p0 = p1;
		p1.x = pmin.x; p1.y = pmax.y;
		border.set(p0, p1);
		if (intersect(border)) {
			return true;
		}

		p0 = p1;
		p1.x = pmin.x; p1.y = pmin.y;
		border.set(p0, p1);
		if (intersect(border)) {
			return true;
		}
	}

	return false;
}

/**
 *
 */
void Framework::Primitive::Polygon2D::addHole(const std::vector<glm::vec2>& hole)
{
	m_holes.push_back(hole);
}

/**
 *
 */
uint32_t Framework::Primitive::Polygon2D::addHole(void)
{
	std::vector<glm::vec2> empty;
	uint32_t i = m_holes.size();
	m_holes.push_back(empty);
	return i;
}

/**
 *
 */
void Framework::Primitive::Polygon2D::addPoint(uint32_t hole, const glm::vec2& p)
{
	m_holes[hole].push_back(p);
}
