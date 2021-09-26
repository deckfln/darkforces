#pragma once

#include <glm/vec2.hpp>

namespace Framework {
	class Segment2D {
		glm::vec2 m_p;
		glm::vec2 m_p1;
		glm::vec2 m_direction;

		float m_dx, m_dy;

	public:
		Segment2D();
		Segment2D(const glm::vec2& p, const glm::vec2& p1);
		Segment2D(float x, float y, float x1, float y1);

		void set(const glm::vec2& p, const glm::vec2& p1);
		inline const glm::vec2& start(void) const { return m_p; };
		inline const glm::vec2& end(void)  const { return m_p1; };
		inline const glm::vec2& direction(void)  const { return m_direction; };

		bool intersect(const Segment2D& segment, glm::vec2& intersection);
	};
}