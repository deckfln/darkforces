#pragma once

#include "../gaEngine/gaActor.h"
#include "../gaEngine/gaComponent/gaSound.h"
#include "../gaEngine/gaComponent/gaCInventory.h"

#include "dfLevel.h"
#include "dfComponent/dfCActor.h"
#include "dfComponent/dfCWeapon.h"
#include "dfComponent/dfComponentLogic.h"
#include "gaItem/dfItem/dfEnergyClip.h"
#include "gaItem/dfItem/dfHeadlight.h"

#include "flightRecorder/frActor.h"

class dfSector;
class dfLevel;

namespace DarkForces {
	/**
	 * Actor for dark forces with the dfSector the actor is
	 */
	class Player : public gaActor {
		DarkForces::Component::Actor m_defaultAI;
		DarkForces::Component::Weapon m_weapon;
		GameEngine::Component::Inventory m_inventory;
		GameEngine::Component::Sound m_sound;

		// predefined items
		DarkForces::Headlight m_headlight = DarkForces::Headlight("headlight");
		DarkForces::Weapon m_pistol;
		DarkForces::EnergyClip m_clip;

		dfLevel* m_level = nullptr;

		DarkForces::Weapon* m_currentWeapon=nullptr;	// current weapon
		bool m_weaponFiring = false;					// weapon is on fire position
		bool m_inMove = false;							// currently moving
		uint32_t m_frameStartMove = 0;					// when did the move start
		float m_wobblingT=0;							// time of wobbling
		float m_wobblingDirection = 1;
		glm::vec3 m_wobbling = glm::vec3(0);			// wobbling the weapon when moving

		void placeWeapon(DarkForces::Weapon* weapon,
			const glm::vec2& delta);			// place the weapon on screen

		void onChangeWeapon(int kweapon);		// Change the current weapon
		void onMove(gaMessage* message);		// when the player moves
		void onFire(gaMessage* message);		// when the player fires
		void onAlarm(gaMessage* message);		// time to display the fire texture is over
		void onLookAt(gaMessage* message);		// when the player looks somewhere
		void onHitBullet(gaMessage* mmessage);	// when the player gets hit by a laser
		void onBulletMiss(gaMessage* mmessage);	// a bullet passed by the player
		void onTogleGogle(gaMessage* mmessage);	// Togle the gogles
		void onTogleHeadlight(gaMessage* mmessage);	// Togle the headlight
		void onAmmo(gaMessage* message);		// display number of ammo

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
		void bind(dfLevel* level);

		bool isOn(const std::string& item);					// the item is present in the inventory and is turned on
		void addItem(Item* item);							// add item to inventory

		void dispatchMessage(gaMessage* message) override;	//

		void setWeapon(DarkForces::Weapon* weapon);			// Change the current weapon
		void setWeapon();									// force the default weapon (pistol)

		int recordSize(void) override {
			return sizeof(flightRecorder::DarkForces::Actor);
		};														// size of one record

		uint32_t recordState(void* record) override;			// return a record of an actor state (for debug)
		void loadState(void* record) override;					// reload an actor state from a record
		void debugGUIChildClass(void) override;					// Add dedicated component debug the entity
	};
}