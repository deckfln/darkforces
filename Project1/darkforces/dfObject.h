#pragma once
#include <string>
#include <list>
#include <stdint.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "../framework/fwAABBox.h"
#include "../gaEngine/gaEntity.h"
#include "../gaEngine/gaCollisionPoint.h"

#include "dfCollision.h"

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
	SCENERY = 1,
	ANIM = 2,
	OFFICER = 4,
	COMMANDO = 8,
	TROOP = 16,
	RED_KEY = 32,
	INTDROID = 64,
	ITEM_SHIELD = 128,
	ITEM_ENERGY = 256,
	LIFE = 512,
	REVIVE = 1024,
	MOUSEBOT = 2048,
	KEY_TRIGGER = 4096,
	ITEM_RIFLE = 8192,
	ITEM_POWER = 16384,
	ITEM_BATTERY = 32768,
	DEAD_MOUSE = 65536
};

/**
 * State of every object
 */
enum class dfState {
	NONE = 0,

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

class dfObject: public gaEntity
{
	int m_is = OBJECT_OBJ;
	int m_objectID = 0;

protected:
	int m_dirtyAnimation = true;			// animation of the object was updated
	int m_dirtyPosition = true;				// position of the object was updated

	glm::vec3 m_position_lvl = glm::vec3(0);// position in level space

	uint32_t m_logics = dfLogic::NONE;		// logic of the object
	int m_difficulty = 0;					// difficulty to be displayed
	float m_ambient = 32.0f;				// ambient light inherited from the sector

	float m_radius = 0;			// This defines the size of an invisible circle around the object where the PLAYER cannot enter or shoot through.
								// *Frames and sprites have radiuses by default*, but 3D objects don't, so you have to set one unless you want the
								// PLAYER to walk right through. You can use this with a Spirit to create an invisible obstacle

	float m_height = 0;			// Similar to radius, height defines an area above (positive value) or below (negative value) an object where you can'twalk or fire through. 
								// Therefore, using radius and height together, you can effectively create an impenetrable cylinder-shaped area around an object


	dfModel* m_source = nullptr;
	dfSector* m_sector = nullptr;						// cached pointer to the sector hosting the object

public:
	dfObject(dfModel *source, const glm::vec3& position, float ambient, int type, uint32_t objectID);
	dfObject(flightRecorder::DarkForces::dfObject *record);

	dfObject* create(void* record) {
		return new dfObject((flightRecorder::DarkForces::dfObject*)record);
	}

	void height(float h) { m_height = h; };
	float height(void) { return m_height; };
	void radius(float r) { m_radius = r; };
	float radius(void) { return m_radius; };
	int difficulty(void);
	void difficulty(int difficulty) { m_difficulty = difficulty; };
	bool collision(void);
	bool named(std::string name);
	bool is(int type);
	bool isLogic(uint32_t logic);
	const std::string& model(void);
	void logic(uint32_t logic);
	void drop(uint32_t logic);				// object to drop in the scene at the current position
	dfSector* sector(void) { return m_sector; };
	void sector(dfSector* s) { m_sector = s; };
	dfSuperSector* superSector(void) override;

	bool checkCollision(fwCylinder& bounding,
		glm::vec3& direction,
		glm::vec3& intersection,
		std::list<gaCollisionPoint>& collisions) override;	// extended collision test after a sucessfull AABB collision
	void moveTo(const glm::vec3& position) override;	// update the object position
	void updateWorldAABB(void) override;				// update the world AABB based on position
	bool update(time_t t) override;						// update based on timer

	// flight recorder and debugger
	inline int recordSize(void) override {
		return sizeof(flightRecorder::DarkForces::dfObject);
	}													// size of one record
	uint32_t recordState(void* record) override;			// return a record of the entity state (for debug)
	void loadState(void* record) override;// reload an entity state from a record

	void debugGUIChildClass(void ) override;			// Add dedicated component debug the entity

	~dfObject();
};