#pragma once

#include "../dfObject.h"
#include <glm/vec4.hpp>

class dfWAX;

class dfSprite: public dfObject
{
	int m_state = 0;			// state of the object for WAX, unused for others
	int m_frame = 0;			// current frame to display based on frameSpeed
	glm::vec3 m_direction = glm::vec3(0);		// direction the object is looking to
	time_t m_lastFrame = 0;		// time of the last frame

public:
	dfSprite(dfWAX* wax, glm::vec3& position, float ambient);
	void state(int state) { m_state = state; };
	void rotation(glm::vec3& rotation);
	virtual bool updateSprite(glm::vec3* position, glm::vec4* texture, glm::vec3* direction);
	virtual bool update(time_t t);		// update based on timer
	~dfSprite();
};