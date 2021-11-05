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
	class Player : public gaActor {
		DarkForces::Component::Actor m_defaultAI;
		GameEngine::Component::Sound m_sound;
		DarkForces::Component::Weapon m_weapon;

		dfLevel* m_level = nullptr;
		bool m_headlight = false;

		DarkForces::Weapon::Kind m_currentWeapon;		// current weapon
		bool m_weaponFiring = false;					// weapon is on fire position
		bool m_inMove = false;							// currently moving
		uint32_t m_frameStartMove = 0;					// when did the move start
		float m_wobblingT=0;							// time of wobbling
		float m_wobblingDirection = 1;
		glm::vec3 m_wobbling = glm::vec3(0);			// wobbling the weapon when moving

		void placeWeapon(DarkForces::Weapon::Kind weapon,
			const glm::vec2& delta);			// place the weapon on screen

		void onChangeWeapon(int kweapon);		// Change the current weapon
		void onMove(gaMessage* message);		// when the player moves
		void onFire(gaMessage* message);		// when the player fires
		void onAlarm(gaMessage* message);		// time to display the fire texture is over
		void onLookAt(gaMessage* message);		// when the player looks somewhere

	public:
		Player(
			int mclass,
			const std::string& name,			// name of the actor
			fwCylinder& cylinder,				// collision cylinder
			const glm::vec3& feet,				// position of the feet in world space
			float eyes,							// distance from the feet to the eyes (camera view)
			float ankle							// distance from the feet to the ankles (can step over)
		);
		Player(flightRecorder::Entity* record);

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