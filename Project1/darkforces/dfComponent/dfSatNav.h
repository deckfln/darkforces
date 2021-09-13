#pragma once
#include <vector>

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
				SEARCH_TRIGGER,
				NOGO
			};

			Status m_status = Status::STILL;
			std::vector<glm::vec3> m_destinations;
			glm::vec3 m_currentDestination = glm::vec3(+INFINITY, +INFINITY, +INFINITY);

			std::list<dfLogicTrigger*> m_triggers;
			dfLogicTrigger* m_targetTrigger = nullptr;

			std::vector<InfElevator*> m_nextElevator;

			void goto_next_trigger(bool first);					// set the destination to the next trigger that can activate an elevator
			void activate_trigger(void);						// activate the current targeted trigger
		public:
			SatNav(float speed);
			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
		};
	}
}