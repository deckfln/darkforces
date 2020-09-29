#pragma once

#include <glm/vec3.hpp>

namespace Framework
{
	class Segment
	{
	public:
		glm::vec3 m_start;
		glm::vec3 m_end;

		Segment(void);
		Segment(const glm::vec3& start, const glm::vec3& end);
		Segment(glm::vec3 *start, glm::vec3 *end);

		float length(void);
	};
}