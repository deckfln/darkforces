#pragma once

#include "../gaEngine/gaActor.h"
#include "flightRecorder/frActor.h"

class dfSector;
class dfLevel;

namespace DarkForces {
	/**
	 * Actor for dark forces with the dfSector the actor is
	 */
	class Actor : public gaActor {
		dfSector* m_currentSector = nullptr;	// in what sector is the sector located
		dfLevel* m_level = nullptr;

	public:
		Actor(
			int mclass,
			const std::string& name,		// name of the actor
			fwCylinder& cylinder,			// collision cylinder
			const glm::vec3& feet,			// position of the feet in world space
			float eyes,						// distance from the feet to the eyes (camera view)
			float ankle						// distance from the feet to the ankles (can step over)
		);
		Actor(flightRecorder::Entity* record);

		void dispatchMessage(gaMessage* message) override;
		void bind(dfLevel* level) { m_level = level; };

		int recordSize(void) override {
			return sizeof(flightRecorder::DarkForces::Actor);
		};														// size of one record

		void recordState(void* record) override;				// return a record of an actor state (for debug)
		void loadState(flightRecorder::Entity* record) override;// reload an actor state from a record
		void debugGUIChildClass(void) override;					// Add dedicated component debug the entity
	};
}