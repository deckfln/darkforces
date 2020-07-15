#pragma once

#include "dfSprite.h"
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

class dfWAX;

class dfSpriteAnimated: public dfSprite
{
	int m_state = 0;						// state of the object for WAX, unused for others
	int m_frame = 0;						// current frame to display based on frameSpeed
	glm::vec3 m_direction = glm::vec3(0);	// direction the object is looking to
	time_t m_lastFrame = 0;					// time of the last animation frame
	time_t m_currentFrame = 0;				// time of the current animation frame
	time_t t;

public:
	dfSpriteAnimated(dfWAX* wax, glm::vec3& position, float ambient);
	void state(int state);
	void rotation(glm::vec3& rotation);

	virtual bool updateSprite(glm::vec3* position, 
		glm::vec4* texture, 
		glm::vec3* direction);
	virtual bool update(time_t t);			// update based on timer
	virtual void dispatchMessage(gaMessage* message);	// let an entity deal with a situation

	~dfSpriteAnimated();
};