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

protected:
	bool m_loopAnimation = true;			// animations are looping (default)

public:
	dfSpriteAnimated(dfWAX* wax, const glm::vec3& position, float ambient);
	dfSpriteAnimated(const std::string& model, const glm::vec3& position, float ambient);
	void state(int state);
	void rotation(const glm::vec3& rotation);

	void loop(bool l) { m_loopAnimation = l; };
	bool loop(void) { return m_loopAnimation; };

	bool updateSprite(glm::vec3* position, 
		glm::vec4* texture, 
		glm::vec3* direction) override;
	bool update(time_t t) override;						// update based on timer
	void dispatchMessage(gaMessage* message) override;	// let an entity deal with a situation
	void OnWorldInsert(void) override;					// trigger when inserted in a gaWorld

	~dfSpriteAnimated();
};