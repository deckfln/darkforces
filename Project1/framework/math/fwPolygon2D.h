#pragma once

#include <vector>
#include <glm/vec2.hpp>

#include "fwAABBox2D.h"

namespace Framework {
	namespace Primitive {
		class Polygon2D {
			std::vector<glm::vec2> m_points;
			std::vector<std::vector<glm::vec2>> m_holes;
			AABBox2D m_aabb;

		public:
			Polygon2D(void);
			Polygon2D(const std::vector<glm::vec2>& points);
			Polygon2D(const glm::vec2* points, uint32_t nb);

			void addPoint(const glm::vec2& p);
			bool isPointInside(const glm::vec2& p);
			void addHole(const std::vector<glm::vec2>& hole);
			uint32_t addHole(void);
			void addPoint(uint32_t hole, const glm::vec2& p);
		};
	}
}