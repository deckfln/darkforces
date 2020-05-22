#include "dfSprite.h"

#include <glm/trigonometric.hpp>

#include "../dfModel/dfWAX.h"

dfSprite::dfSprite(dfWAX* wax, glm::vec3& position, float ambient):
	dfObject(wax, position, ambient)
{
}

bool dfSprite::updateSprite(glm::vec3* position, glm::vec4* texture, glm::vec3* direction)
{
	dfObject::updateSprite(position, texture, direction);

	direction->x = m_direction.x;
	direction->y = m_direction.z;	// level space to gl space
	direction->z = m_direction.y;

	texture->g = (float)m_state;
	texture->b = (float)m_frame;

	return false;
}

/**
 * Create a direction vector ased on pch, yaw, rol
 */
void dfSprite::rotation(glm::vec3& rotation)
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
bool dfSprite::update(time_t t)
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
			m_lastFrame = t;
			return true;
		}
	}

	return false;
}

dfSprite::~dfSprite()
{
}
