#pragma once

#include "../gaEngine/gaActor.h"
#include "../gaEngine/gaComponent/gaSound.h"
#include "../gaEngine/gaComponent/gaCInventory.h"

#include "dfLevel.h"
#include "dfComponent/dfCActor.h"
#include "dfComponent/dfCWeapon.h"
#include "dfComponent/dfComponentLogic.h"
#include "dfComponent/dfCAutoMap.h"
#include "dfComponent/dfCHUDLeft.h"
#include "dfComponent/dfCHUDRight.h"
#include "dfComponent/dfCHUDtext.h"
#include "gaItem/dfItem/dfEnergyClip.h"
#include "gaItem/dfItem/dfHeadlight.h"

#include "flightRecorder/frActor.h"

class dfSector;
class dfLevel;
class fwScene;

namespace DarkForces {
	/**
	 * Actor for dark forces with the dfSector the actor is
	 */
	class Player : public gaActor {
		DarkForces::Component::Actor m_defaultAI;
		DarkForces::Component::Weapon m_weapon;
		GameEngine::Component::Inventory m_inventory;
		GameEngine::Component::Sound m_sound;
		DarkForces::Component::AutoMap m_automap;
		DarkForces::Component::HUDLeft m_health;
		DarkForces::Component::HUDRight m_ammo;
		DarkForces::Component::HUDtext m_text;

		// predefined items
		DarkForces::Headlight m_headlight = DarkForces::Headlight("headlight");
		DarkForces::Weapon m_pistol;
		DarkForces::Weapon m_rifle;
		DarkForces::EnergyClip m_clip;

		dfLevel* m_level = nullptr;

		DarkForces::Weapon* m_currentWeapon=nullptr;	// current weapon
		bool m_weaponFiring = false;					// weapon is on fire position
		uint32_t m_frameStartMove = 0;					// when did the move start
		float m_wobblingT=0;							// time of wobbling
		float m_wobblingDirection = 1;
		glm::vec3 m_wobbling = glm::vec3(0);			// wobbling the weapon when moving

		void onChangeWeapon(int kweapon);		// Change the current weapon
		void onMove(gaMessage* message);		// when the player moves
		void onHitBullet(gaMessage* mmessage);	// when the player gets hit by a laser
		void onBulletMiss(gaMessage* mmessage);	// a bullet passed by the player
		void onTogleGogle(gaMessage* mmessage);	// Togle the gogles
		void onTogleHeadlight(gaMessage* mmessage);	// Togle the headlight
		void onAmmo(gaMessage* message);		// display number of ammo
		void onShield(gaMessage* message);		// display number of shield
		void onLife(gaMessage* message);		// display live status
		void onAddItem(gaMessage* message);		// add item on the PDA
		void onShowPDA(gaMessage* message);		// display the PDA
		void onShowAutomap(gaMessage* message);	// display/hide the automap
		void onCompleteGoal(gaMessage* message);// when a goal is complete
		void onScreenResize(gaMessage* message);// when the screen gets resized

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
		inline DarkForces::Component::AutoMap* automap(void) { return &m_automap; };

		bool isOn(const std::string& item);					// the item is present in the inventory and is turned on
		void addItem(Item* item);							// add item to inventory

		void dispatchMessage(gaMessage* message) override;	//

		void setScene(fwScene* scene);						// add component on screen

		// flight recorder
		int recordSize(void) override {
			return sizeof(flightRecorder::DarkForces::Actor);
		};														// size of one record

		uint32_t recordState(void* record) override;			// return a record of an actor state (for debug)
		void loadState(void* record) override;					// reload an actor state from a record

		// debugger
		void debugGUIChildClass(void) override;					// Add dedicated component debug the entity
	};
}