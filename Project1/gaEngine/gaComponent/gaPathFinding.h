#pragma once

#include "../gaComponent.h"

#include <glm/vec3.hpp>

namespace GameEngine {
	namespace Component {
		class PathFinding : public gaComponent {
			enum class Status
			{
				STILL,
				MOVE
			};
			Status m_status = Status::STILL;
			glm::vec3 m_destination = glm::vec3(0);	
			float m_speed=0;
			GameEngine::Transform* m_transforms = nullptr;		// transforms to move the object

		public:
			PathFinding(float speed);
			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
			void debugGUIinline(void) override;					// display the component in the debugger
		};
	}
}