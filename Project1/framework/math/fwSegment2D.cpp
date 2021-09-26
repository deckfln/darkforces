#include "fwSegment2D.h"

Framework::Segment2D::Segment2D():
	m_p(0,0),
	m_p1(0,0)
{
    m_dx = 0;
    m_dy = 0;
}

Framework::Segment2D::Segment2D(const glm::vec2& p, const glm::vec2& p1):
	m_p(p),
	m_p1(p1)
{
    m_dx = m_p1.x - m_p.x;
    m_dy = m_p1.y - m_p.y;

    m_direction.x = m_dx;
    m_direction.y = m_dy;
}

Framework::Segment2D::Segment2D(float x, float y, float x1, float y1):
    m_p(x, y),
    m_p1(x1, y1),
    m_dx(m_p1.x - m_p.x),
    m_dy(m_p1.y - m_p.y)
{
    m_direction.x = m_dx;
    m_direction.y = m_dy;
}

void Framework::Segment2D::set(const glm::vec2& p, const glm::vec2& p1)
{
	m_p = p;
	m_p1 = p1;

    m_dx = m_p1.x - m_p.x;
    m_dy = m_p1.y - m_p.y;

    m_direction.x = m_dx;
    m_direction.y = m_dy;
}

bool Framework::Segment2D::intersect(const Segment2D& segment, glm::vec2& intersection)
{
    //http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
    // Returns 1 if the lines intersect, otherwise 0. In addition, if the lines 
    // intersect the intersection point may be stored in the floats i_x and i_y.
    float s, t;
    float slop = -segment.m_dx * m_dy + m_dx * segment.m_dy;

    s = (-m_dy * (m_p.x - segment.m_p.x) + m_dx * (m_p.y - segment.m_p.y)) / slop;
    t = (segment.m_dx * (m_p.y - segment.m_p.y) - segment.m_dy * (m_p.x - segment.m_p.x)) / slop;

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        intersection.x = m_p.x + (t * m_dx);
        intersection.y = m_p.y + (t * m_dy);
        /*
        printf("intersect:%f,%f,%f,%f,%f,%f\n",
            m_p.x, m_p.y,
            m_p1.x, m_p1.y,
            intersection.x, intersection.y);
        */
        return true;
    }

    return false; // No collision        
}
