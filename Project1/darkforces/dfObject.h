#pragma once
#include <string>
#include <glm/vec3.hpp>

class dfModel;

enum {
	DF_LOGIC_NONE,
	DF_LOGIC_SCENERY = 1,
	DF_LOGIC_ANIM = 2,
	DF_LOGIC_I_OFFICER = 4,
	DF_LOGIC_COMMANDO = 8,
	DF_LOGIC_TROOP = 16,
	DF_LOGIC_RED_KEY = 32,
	DF_LOGIC_INTDROID = 64,
	DF_LOGIC_ITEM_SHIELD = 128,
	DF_LOGIC_ITEM_ENERGY = 256,
	DF_LOGIC_LIFE = 512,
	DF_LOGIC_REVIVE = 1024
};

enum {
	// state of i_officer, commando ...
	DF_STATE_ENEMY_MOVE = 0,
	DF_STATE_ENEMY_ATTACK = 1,
	DF_STATE_ENEMY_DIE_FROM_PUNCH = 2,
	DF_STATE_ENEMY_DIE_FROM_SHOT = 3,
	DF_STATE_ENEMY_LIE_DEAD = 4,
	DF_STATE_ENEMY_STAY_STILL = 5,
	DF_STATE_ENEMY_FOLLOW_PRIMARY_ATTACK = 6,
	DF_STATE_ENEMY_SECONDARY_ATTACK = 7,
	DF_STATE_ENEMY_FOLLOW_SECONDARY_ATTACK = 8,
	DF_STATE_ENEMY_JUMP = 9,
	DF_STATE_ENEMY_INJURED = 12,
	DF_STATE_ENEMY_SPECIAL = 13,

	// state of remote
	DF_STATE_REMOTE_MOVE = 0,
	DF_STATE_REMOTE_STAY_STILL = 1,
	DF_STATE_REMOTE_DIE = 2,
	DF_STATE_REMOTE_DIE1 = 3,

	// state of sceneries
	DF_STATE_SCENERY_NORMAL = 0,
	DF_STATE_SCENERY_ATTACK = 1,

	// state of barrel
	DF_STATE_BARREL_NORMAL = 0,
	DF_STATE_BARREL_EXPLODE = 1
};

// list of all enemies
const unsigned long DF_ENEMIES = DF_LOGIC_I_OFFICER | DF_LOGIC_COMMANDO | DF_LOGIC_TROOP;

class dfObject
{
protected:
	float m_x=0, m_y=0, m_z=0;	// position in level space
	glm::vec3 m_direction;		// direction the object is looking to

	int m_logics = 0;			// logic of the object
	int m_difficulty = 0;		// difficulty to be displayed

	float m_radius = 0;			// This defines the size of an invisible circle around the object where the PLAYER cannot enter or shoot through.
								// *Frames and sprites have radiuses by default*, but 3D objects don't, so you have to set one unless you want the
								// PLAYER to walk right through. You can use this with a Spirit to create an invisible obstacle

	float m_height = 0;			// Similar to radius, height defines an area above (positive value) or below (negative value) an object where you can'twalk or fire through. 
								// Therefore, using radius and height together, you can effectively create an impenetrable cylinder-shaped area around an object

	int m_state = 0;			// state of the object for WAX, unused for others
	int m_frame = 0;			// current frame to display based on frameSpeed
	time_t m_lastFrame = 0;

	dfModel* m_source;
public:
	dfObject(dfModel *source, float x, float y, float z);
	void set(float pch, float yaw, float rol, int difficulty);
	void height(float h) { m_height = h; };
	float height(void) { return m_height; };
	void radius(float r) { m_radius = r; };
	float radius(void) { return m_radius; };
	bool named(std::string name);
	int difficulty(void);
	std::string& model(void);
	bool updateSprite(glm::vec3* position, glm::vec3* texture, glm::vec3* direction);
	bool update(time_t t);		// update based on timer
	void logic(int logic);
	~dfObject();
};