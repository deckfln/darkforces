#include "dfSpriteAnimated.h"

#include <glm/trigonometric.hpp>

#include "../dfModel/dfWAX.h"
#include "../dfLevel.h"

dfSpriteAnimated::dfSpriteAnimated(dfWAX* wax, glm::vec3& position, float ambient):
	dfSprite(wax, position, ambient, OBJECT_WAX)
{
	gaEntity::updateWorldAABB();
}

/**
 * Change the state of an object
 */
void dfSpriteAnimated::state(int state)
{
	m_state = state;
	modelAABB(((dfWAX*)m_source)->bounding(m_state));
	dfSprite::updateWorldAABB();
}

bool dfSpriteAnimated::updateSprite(glm::vec3* position, glm::vec4* texture, glm::vec3* direction)
{
	if (m_dirtyPosition) {
		direction->x = m_direction.x;
		direction->y = m_direction.z;	// level space to gl space
		direction->z = m_direction.y;
	}

	if (m_dirtyAnimation) {
		texture->g = (float)m_state;
		texture->b = (float)m_frame;
	}

	return dfSprite::updateSprite(position, texture, direction);
}

/**
 * Create a direction vector ased on pch, yaw, rol
 */
void dfSpriteAnimated::rotation(glm::vec3& rotation)
{
	// YAW = value in degrees where 0 is at the "top of the screen when you look at the map". The value increases clockwise
	float yaw = glm::radians(rotation.y);
	m_direction.x = sin(yaw);	// in level space
	m_direction.y = -cos(yaw);
	m_direction.z = 0;
}

/**
 * Animate the object frame
 */
bool dfSpriteAnimated::update(time_t t)
{
	if (m_logics & DF_LOGIC_ANIM) {
		int frameRate = m_source->framerate(m_state);
		if (frameRate == 0) {
			// static objects like FME are not updated
			return false;
		}

		time_t frameTime = 1000 / frameRate; // time of one frame in milliseconds

		time_t delta = t - m_lastFrame;
		if (delta >= frameTime) {
			m_frame = m_source->nextFrame(m_state, m_frame);
			if (m_frame == -1) {
				// when we loop back, first take some action
				switch (m_state) {
				case DF_STATE_ENEMY_DIE_FROM_PUNCH:
				case DF_STATE_ENEMY_DIE_FROM_SHOT:
					m_state = DF_STATE_ENEMY_LIE_DEAD;
					m_frame = 0;
					m_dirtyAnimation = true;
					m_physical = false;			// remove collision box, they actor is dead
					return false;
				}
			}

			m_lastFrame = t;
			m_dirtyAnimation = true;

			return true;
		}
	}

	return false;
}

dfSpriteAnimated::~dfSpriteAnimated()
{
}