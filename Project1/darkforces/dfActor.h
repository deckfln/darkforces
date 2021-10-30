#pragma once

#include "../gaEngine/gaActor.h"
#include "dfComponent/dfComponentActor.h"
#include "../gaEngine/gaComponent/gaSound.h"

#include "dfLevel.h"
#include "dfComponent/dfWeapon.h"

#include "flightRecorder/frActor.h"

class dfSector;
class dfLevel;

namespace DarkForces {
	/**
	 * Actor for dark forces with the dfSector the actor is
	 */
	class Actor : public gaActor {
		DarkForces::Component::Actor m_defaultAI;
		GameEngine::Component::Sound m_sound;
		DarkForces::Component::Weapon m_weapon;

		dfLevel* m_level = nullptr;
		bool m_headlight = false;

		void onChangeWeapon(int kweapon);	// Change the current weapon

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

		// getter/setter
		inline bool headlight(void) { return m_headlight; };
		void bind(dfLevel* level);

		void dispatchMessage(gaMessage* message) override;		//

		void setWeapon(DarkForces::Weapon::Kind);	// Change the current weapon

		int recordSize(void) override {
			return sizeof(flightRecorder::DarkForces::Actor);
		};														// size of one record

		uint32_t recordState(void* record) override;			// return a record of an actor state (for debug)
		void loadState(void* record) override;					// reload an actor state from a record
		void debugGUIChildClass(void) override;					// Add dedicated component debug the entity
	};
}