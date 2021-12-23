#pragma once

#include "../gaBehaviorNode.h"

class gaEntity;
class fwGeometry;
class fwMesh;

namespace GameEngine {
	namespace Behavior {
		class MoveTo : public BehaviorNode {
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
			std::vector<glm::vec3>* m_navpoints;				// nav points for the move
			uint32_t m_currentNavPoint = 0;						// beware, backtrack as navpoints are in reverse order
			std::vector<glm::vec3> m_previous;					// previous positions (circular buffer)
			uint32_t m_previous_current = 0;
			uint32_t m_previous_size = 0;
			uint32_t m_moveID;									// ID of the last raised WANT_TO_MOVE message

			std::map<std::string, bool> m_exit;					// list of variables triggering exit of the loop

#ifdef _DEBUG
			void debug(void);									// init debug mode
			bool m_debug = false;								// display navpoints in opengl
			fwGeometry* m_geometry=nullptr;
			float* m_vertices = nullptr;
			fwMesh* m_mesh = nullptr;

#endif

			glm::vec3 nextWayPoint(bool normalize);				// return the direction to the next way point
			void triggerMove(void);								// send the move messages
			void triggerMove(const glm::vec3& direction);		// send the move messages

			bool conditionMet(void);							// check exit conditions
			void onReachedNextWayPoint(gaMessage* message);		// select the next waypoint or end the movement
			void onBlockedWay(gaMessage* message);				// cancel the satnav because there is a obstable in front

			void onMove(gaMessage* message);
			void onCollide(gaMessage* message);
			void onCancel(gaMessage* message);
		public:
			MoveTo(void);
			MoveTo(const char* name);
			MoveTo(const char* name, float speed);

			// getter/setter
			inline void speed(float speed) { m_speed = speed; };

			void dispatchMessage(gaMessage* message, Action *r) override;	// let a component deal with a situation
			void init(void *) override;

			// Behavior engine, GameEngine::BehaviorNode* used
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			//debugger
			void debugGUInode(void) override;							// display the component in the debugger

			// flight recorder status
			uint32_t recordState(void* record) override;				// save the component state in a record
			uint32_t loadState(void* record) override;					// reload a component state from a record
		};
	}

	namespace FlightRecorder {
		struct MoveTo {
			BehaviorNode node;
			uint32_t status;
			uint32_t nbPrevious;
			uint32_t current;
			char lastCollision[64];
		};
	}
}