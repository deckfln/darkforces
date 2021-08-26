#pragma once

#include "../../framework/math/Segment.h"

#include "../gaComponent.h"
#include "../Collider.h"

#include <glm/vec3.hpp>

namespace GameEngine {
	namespace Component {
		class ActiveProbe : public gaComponent {
			glm::vec3 m_direction;
			Framework::Segment m_segment;
			glm::mat4x4 m_worldMatrix;
			glm::mat4x4 m_inverseWorldMatrix;
			Collider m_collider;								// collider for the segment
			fwAABBox m_worldAABB;								

		public:
			ActiveProbe(void);
			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
			void debugGUIinline(void) override;					// display the component in the debugger
		};
	}
}