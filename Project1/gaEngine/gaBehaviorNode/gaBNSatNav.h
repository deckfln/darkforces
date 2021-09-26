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
			std::vector<glm::vec3> m_previous;					// previous positions (circular buffer)
			uint32_t m_previous_current = 0;
			uint32_t m_previous_size = 0;

			glm::vec3 nextWayPoint(bool normalize);				// return the direction to the next way point
			void triggerMove(void);								// send the move messages
			void triggerMove(const glm::vec3& direction);		// send the move messages

			void onReachedNextWayPoint(gaMessage* message);		// select the next waypoint or end the movement
			void onBlockedWay(gaMessage* message);				// cancel the satnav because there is a obstable in front

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

			// flight recorder status
			uint32_t recordState(void* record) override;				// save the component state in a record
			uint32_t loadState(void* record) override;					// reload a component state from a record
		};
	}

	namespace FlightRecorder {
		struct SatNav {
			BehaviorNode node;
			uint32_t status;
			glm::vec3 destination;
			uint32_t nbNavPoints;
			uint32_t nbPrevious;
			uint32_t current;
			char lastCollision[64];
			glm::vec3 points[1];
		};
	}
}