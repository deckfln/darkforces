#pragma once

#include "../gaComponent.h"

#include <glm/vec3.hpp>
#include <vector>

namespace GameEngine {
	namespace Component {
		class SatNav : public gaComponent {
			enum class Status {
				STILL,
				MOVE_TO_NEXT_WAYPOINT,
				NEARLY_REACHED_NEXT_WAYPOINT,
				REACHED_NEXT_WAYPOINT,
			};

			Status m_status = Status::STILL;
			glm::vec3 m_destination = glm::vec3(0);	
			float m_speed=0;
			GameEngine::Transform* m_transforms = nullptr;		// transforms to move the object
			std::vector<glm::vec3> m_navpoints;					// nav points for the move
			uint32_t m_currentNavPoint=0;						// beware, backtrack as navpoints a	re in reverse order
			std::vector<glm::vec3> m_previous;					// previous position
			glm::vec3 nextWayPoint(bool normalize);				// return the direction to the next way point
			void triggerMove(void);								// send the move messages
			void triggerMove(const glm::vec3& direction);		// send the move messages

		public:
			SatNav(float speed);

			// flight recorder status
			inline uint32_t recordSize(void);					// size of the component record
			uint32_t recordState(void* record);					// save the component state in a record
			uint32_t loadState(void* record);					// reload a component state from a record

			// debugger
			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
			void debugGUIinline(void) override;					// display the component in the debugger
		};
	}
}