#include "Segment.h"

#include <glm/glm.hpp>

Framework::Segment::Segment(void) :
	m_start(0),
	m_end(0)
{
}

Framework::Segment::Segment(const glm::vec3& start, const glm::vec3& end):
	m_start(start),
	m_end(end)
{
}

Framework::Segment::Segment(glm::vec3* start, glm::vec3* end):
	m_start(*start),
	m_end(*end)
{
}

void Framework::Segment::set(const glm::vec3& start, const glm::vec3& end)
{
	m_start = start;
	m_end = end;
}

float Framework::Segment::length(void)
{
	return glm::length(m_end - m_start);
}
