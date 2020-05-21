#pragma once
#include <string>
#include <glm/vec3.hpp>

class dfModel;

enum {
	DF_LOGIC_SCENERY = 1,
	DF_LOGIC_ANIM = 2,
	DF_LOGIC_I_OFFICER = 4,
	DF_LOGIC_COMMANDO = 8
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
const unsigned long DF_ENEMIES = DF_LOGIC_I_OFFICER | DF_LOGIC_COMMANDO;

class dfObject
{
protected:
	float m_x=0, m_y=0, m_z=0;	// position in level space
	glm::vec3 m_direction;		// direction the object is looking to

	int m_logics = 0;			// logic of the object
	int m_difficulty = 0;

	int m_state = 0;			// state of the object for WAX, unused for others
	int m_frame = 0;			// current frame to display based on frameSpeed
	time_t m_lastFrame = 0;

	dfModel* m_source;
public:
	dfObject(dfModel *source, float x, float y, float z);
	void set(float pch, float yaw, float rol, int difficulty);
	bool named(std::string name);
	int difficulty(void);
	std::string& model(void);
	bool updateSprite(glm::vec3* position, glm::vec3* texture, glm::vec3* direction);
	bool update(time_t t);		// update based on timer
	void logic(int logic);
	~dfObject();
};