#pragma once
#include <vector>
#include <map>
#include "../../gaEngine/gaComponent/gaSatNav.h"

class dfLogicTrigger;
namespace DarkForces
{
	namespace Component
	{
		class InfElevator;

		class SatNav : public GameEngine::Component::SatNav
		{
			enum class Status {
				STILL,
				MOVE_TO_DESTINATION,
				REACHED_DESTINATION,
				WAIT_DOOR,
				GOTO_FIRST_TRIGGER,
				GOTO_NEXT_TRIGGER,
				NOGO,
			};

			Status m_status = Status::STILL;
			std::vector<glm::vec3> m_destinations;
			glm::vec3 m_currentDestination = glm::vec3(+INFINITY, +INFINITY, +INFINITY);

			std::list<dfLogicTrigger*> m_triggers;
			dfLogicTrigger* m_targetTrigger = nullptr;

			std::vector<InfElevator*> m_nextElevator;

			void goto_next_trigger(void);						// set the destination to the next trigger that can activate an elevator
			void activate_trigger(void);						// activate the current targeted trigger

			std::map<uint32_t, bool (DarkForces::Component::SatNav::*)(gaMessage *msg)> m_messages;

			bool onSatNav_goto(gaMessage* message);
			bool onCollide(gaMessage* message);					// deal with collide message
			bool onSatNav_wait(gaMessage* message);				// wait for a door to open
			bool onSatNav_Reached(gaMessage* message);			// satnav reached its destination

		public:
			SatNav(float speed);
			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
		};
	}
}