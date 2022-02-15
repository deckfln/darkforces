#pragma once

#include <glm/vec2.hpp>
#include "fwSegment2D.h"

namespace Framework {
	class AABBox2D {
		glm::vec2 m_p;
		glm::vec2 m_p1;

		bool alignedPlan(float t, const Framework::Segment2D& segment, float& t1, glm::vec2& p);
		bool xAlignedPlan(float x, const Framework::Segment2D& segment, float& t1, glm::vec2& p);
		bool yAlignedPlan(float y, const Framework::Segment2D& segment, float& t1, glm::vec2& p);

	public:
		AABBox2D();
		AABBox2D(const glm::vec2& p, const glm::vec2& p1);
		AABBox2D(float x, float y, float x1, float y1);
		AABBox2D(const Framework::Segment2D& segment);

		inline const glm::vec2& min(void) { return m_p; };
		inline const glm::vec2& max(void) { return m_p1; };

		void set(float, float, float, float);				// build from coordinates
		void set(const glm::vec2& p, const glm::vec2& p1);	// build from 2 points
		void set(const glm::vec2* v, uint32_t nb);			// build from vertices

		bool intersect(const Framework::Segment2D& segment, glm::vec2& p);	// intersect with a segment
		bool intersect(const AABBox2D& aabb);				// intersect with a box
		bool inside(const glm::vec2& p);					// is point inside
		bool inside(const AABBox2D& box);					// is "this" inside "box"
		void extend(const glm::vec2& p);					// extend the AABB using the point
	};
}