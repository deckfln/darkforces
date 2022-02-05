#include "fwPolygon2D.h"

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
		if ((m_points[i].y > p.y) != (m_points[j].y >= p.y) &&
			p.x <= (m_points[j].x - m_points[i].x) * (p.y - m_points[i].y) / (m_points[j].y - m_points[i].y) + m_points[i].x)
		{
			inside = !inside;
		}
	}

	return inside;
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
