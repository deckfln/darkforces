#pragma once
#include <string>
#include <list>
#include <stdint.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <stack>

#include "../framework/fwAABBox.h"
#include "../gaEngine/gaEntity.h"
#include "../gaEngine/gaCollisionPoint.h"

#include "gaItem/dfItem.h"
#include "dfCollision.h"
#include "dfConfig.h"
#include "dfComponent/dfComponentLogic.h"
#include "flightRecorder/frObject.h"

class fwScene;
class fwMesh;
class fwCylinder;

class dfModel;
class dfSector;

/**
 * Logic for objects
 */
enum dfLogic {
	NONE,
	SCENERY = 1<<1,
	ANIM = 1<<2,
	OFFICER = 1<<3,
	COMMANDO = 1<<4,
	TROOP = 1<<5,
	RED_KEY = 1<<6,
	INTDROID = 1<<7,
	ITEM_SHIELD = 1<<8,
	ITEM_ENERGY = 1<<9,
	LIFE = 1<<10,
	REVIVE = 1<<11,
	MOUSEBOT = 1<<12,
	KEY_TRIGGER = 1<<13,
	ITEM_RIFLE = 1<<14,
	ITEM_POWER = 1<<15,
	ITEM_BATTERY = 1<<16,
	DEAD_MOUSE = 1<<17,
	GOGGLES = 1<<18,
	MEDKIT = 1<<19,
	WEAPON = 1<<20,
	PLANS = 1<<21,			// death star plans
	PHRIK = 1<<22,			// Phrik metal
	NAVA = 1<<23,			// Nava Card
	DATATAPE = 1<<24,		// data tapes 
	DT_WEAPON = 1<<25,		// broken DT weapon
	PILE = 1<<26,			// Your Gear
};

/**
 * State of every object
 */
enum class dfState {
	NONE = -1,

	// state of i_officer, commando ...
	ENEMY_MOVE = 0,
	ENEMY_ATTACK = 1,
	ENEMY_DIE_FROM_PUNCH = 2,
	ENEMY_DIE_FROM_SHOT = 3,
	ENEMY_LIE_DEAD = 4,
	ENEMY_STAY_STILL = 5,
	ENEMY_FOLLOW_PRIMARY_ATTACK = 6,
	ENEMY_SECONDARY_ATTACK = 7,
	ENEMY_FOLLOW_SECONDARY_ATTACK = 8,
	ENEMY_JUMP = 9,
	ENEMY_INJURED = 12,
	ENEMY_SPECIAL = 13,

	// state of remote
	REMOTE_MOVE = 0,
	REMOTE_STAY_STILL = 1,
	REMOTE_DIE = 2,
	REMOTE_DIE1 = 3,

	// state of sceneries
	SCENERY_NORMAL = 0,
	SCENERY_ATTACK = 1,

	// state of barrel
	BARREL_NORMAL = 0,
	BARREL_EXPLODE = 1
};

/**
 * Type of the object
 */
enum {
	OBJECT_OBJ,
	OBJECT_WAX,
	OBJECT_FME,
	OBJECT_3DO
};

// list of all enemies
const uint32_t DF_LOGIC_ENEMIES = dfLogic::OFFICER | dfLogic::COMMANDO | dfLogic::TROOP | dfLogic::INTDROID;

// list of all physical logics
const uint32_t DF_LOGIC_PHYSICAL = DF_LOGIC_ENEMIES | dfLogic::SCENERY;

namespace DarkForces {
	class Object : public gaEntity
	{
		int m_is = OBJECT_OBJ;
		int m_objectID = 0;
		dfState m_state = dfState::NONE;						// state of the object
		std::stack<dfState> m_previousStates;					// push/pop status for loops

	protected:
		int m_dirtyAnimation = true;							// animation of the object was updated
		int m_dirtyPosition = true;								// position of the object was updated

		glm::vec3 m_position_lvl = glm::vec3(0);				// position in level space

		dfComponentLogic m_logic;								// dealing with the logic of the object
		uint32_t m_logics = dfLogic::NONE;						// logic of the object
		int m_difficulty = 0;									// difficulty to be displayed
		float m_ambient = 32.0f;								// ambient light inherited from the sector

		// inherited from gaEntity
		//float m_radius = 0;			// This defines the size of an invisible circle around the object where the PLAYER cannot enter or shoot through.
									// *Frames and sprites have radiuses by default*, but 3D objects don't, so you have to set one unless you want the
									// PLAYER to walk right through. You can use this with a Spirit to create an invisible obstacle

		//float m_height = 0;			// Similar to radius, height defines an area above (positive value) or below (negative value) an object where you can'twalk or fire through. 
									// Therefore, using radius and height together, you can effectively create an impenetrable cylinder-shaped area around an object


		dfModel* m_source = nullptr;
		dfSector* m_sector = nullptr;							// cached pointer to the sector hosting the object

		DarkForces::Item* m_item = nullptr;						// pointer to a an GameEngine::Item

		void onStateChange(dfState state, bool loop);			// direct change of state

	public:
		Object(dfModel* source, const glm::vec3& position, float ambient, int type, uint32_t objectID);
		Object(const std::string& model, const glm::vec3& position);
		Object(const std::string& model, const glm::vec3& position, uint32_t objectID);
		Object(flightRecorder::DarkForces::Object* record);

		Object* create(void* record) {
			return new Object((flightRecorder::DarkForces::Object*)record);
		}

		inline void item(DarkForces::Item* item) { m_item = item; };
		inline DarkForces::Item* item(void) { return m_item; };
		inline void height(float h) { m_height = h; };
		inline float height(void) { return m_height; };
		inline void radius(float r) { m_radius = r; };
		inline float radius(void) { return m_radius; };
		int difficulty(void);
		inline void difficulty(int difficulty) { m_difficulty = difficulty; };
		bool collision(void);
		bool named(std::string name);
		bool is(int type);
		bool isLogic(uint32_t logic);
		const std::string& model(void);
		void logic(uint32_t logic);
		void drop(uint32_t logic, uint32_t value=0);			// object to drop in the scene at the current position
		inline dfSector* sector(void) { return m_sector; };
		inline void sector(dfSector* s) { m_sector = s; };
		dfSuperSector* superSector(void) override;

		/*
		void pushState(dfState state);							// save and restore status
		dfState popState(void);
		*/

		bool checkCollision(fwCylinder& bounding,
			glm::vec3& direction,
			glm::vec3& intersection,
			std::list<gaCollisionPoint>& collisions) override;	// extended collision test after a sucessfull AABB collision
		void moveTo(const glm::vec3& position) override;		// update the object position
		void updateWorldAABB(void) override;					// update the world AABB based on position
		bool update(time_t t) override;							// update based on timer

		void dispatchMessage(gaMessage* message) override;		// let an entity deal with a situation

		virtual void extend(void) {};							// extend the object at the end of the load

		// flight recorder and debugger
		inline int recordSize(void) override {
			return sizeof(flightRecorder::DarkForces::Object);
		}														// size of one record
		uint32_t recordState(void* record) override;			// return a record of the entity state (for debug)
		void loadState(void* record) override;					// reload an entity state from a record

		void debugGUIChildClass(void) override;					// Add dedicated component debug the entity

		~Object();
	};
}
