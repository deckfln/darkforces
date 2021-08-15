#pragma once

#include "dfSprite.h"
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

#include "../flightRecorder/frSpriteAnimated.h"

class dfWAX;

class dfSpriteAnimated: public dfSprite
{
	dfState m_state = dfState::NONE;		// state of the object for WAX, unused for others
	int m_frame = 0;						// current frame to display based on frameSpeed
	glm::vec3 m_direction = glm::vec3(0);	// direction the object is looking to
	time_t m_lastFrame = 0;					// time of the last animation frame
	time_t m_currentFrame = 0;				// time of the current animation frame

protected:
	bool m_loopAnimation = true;			// animations are looping (default)

public:
	dfSpriteAnimated(dfWAX* wax, const glm::vec3& position, float ambient, uint32_t objectID);
	dfSpriteAnimated(const std::string& model, const glm::vec3& position, float ambient);
	dfSpriteAnimated(flightRecorder::DarkForces::SpriteAnimated* record);

	static void* create(void* record) {
		return new dfSpriteAnimated((flightRecorder::DarkForces::SpriteAnimated*)record);
	}

	void state(dfState state);
	void rotation(const glm::vec3& rotation);

	void loop(bool l) { m_loopAnimation = l; };
	bool loop(void) { return m_loopAnimation; };

	bool updateSprite(glm::vec3* position, 
		glm::vec4* texture, 
		glm::vec3* direction) override;
	bool update(time_t t) override;						// update based on timer
	void dispatchMessage(gaMessage* message) override;	// let an entity deal with a situation

	// flight recorder and debugger
	inline int recordSize(void) override {
		return sizeof(flightRecorder::DarkForces::SpriteAnimated);
	}													// size of one record
	uint32_t recordState(void* record) override;			// return a record of the entity state (for debug)
	void loadState(flightRecorder::Entity* record) override;// reload an entity state from a record

	void debugGUIChildClass(void) override;			// Add dedicated component debug the entity
	~dfSpriteAnimated();
};