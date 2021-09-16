#pragma once

#include "../gaBehaviorNode.h"

class gaEntity;

namespace GameEngine {
	namespace Behavior {
		class SatNav : public BehaviorNode {
			enum class Status {
				STILL,
				MOVE_TO_NEXT_WAYPOINT,
				NEARLY_REACHED_NEXT_WAYPOINT,
				REACHED_NEXT_WAYPOINT,
			};

			Status m_status = Status::STILL;
			glm::vec3 m_destination = glm::vec3(0);
			float m_speed = 0;
			GameEngine::Transform* m_transforms = nullptr;		// transforms to move the object
			std::vector<glm::vec3> m_navpoints;					// nav points for the move
			uint32_t m_currentNavPoint = 0;						// beware, backtrack as navpoints a	re in reverse order
			std::vector<glm::vec3> m_previous;					// previous position

			glm::vec3 nextWayPoint(bool normalize);				// return the direction to the next way point
			void triggerMove(void);								// send the move messages
			void triggerMove(const glm::vec3& direction);		// send the move messages

			void onGoto(gaMessage* message);
			void onMove(gaMessage* message);
			void onCollide(gaMessage* message);
			void onCancel(gaMessage* message);
		public:
			SatNav(void);
			SatNav(const char* name);
			SatNav(const char* name, float speed);

			// getter/setter
			inline void speed(float speed) { m_speed = speed; };

			BehaviorNode* dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
			void init(void *) override;
		};
	}
}