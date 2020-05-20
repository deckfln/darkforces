#include "dfObject.h"

#include "dfModel.h"
#include "dfSprites.h"
#include "dfLevel.h"

dfObject::dfObject(dfModel *source, float x, float y, float z):
	m_source(source),
	m_x(x),
	m_y(y),
	m_z(z)
{
}

/**
 * set object attributes
 */
void dfObject::set(float pch, float yaw, float rol, int difficulty)
{
	// YAW = value in degrees where 0 is at the "top of the screen when you look at the map". The value increases clockwise

	yaw = glm::radians(yaw);
	m_direction.x = cos(yaw);	// in level space
	m_direction.y = sin(yaw);
	m_direction.z = 0;

	m_difficulty = difficulty;
}

/**
 * Check the name of th associated WAX
 */
bool dfObject::named(std::string name)
{
	return m_source->named(name);
}

int dfObject::difficulty(void)
{
	return m_difficulty + 3;
}

/**
 * get the name of the model the object is based on
 */
std::string& dfObject::model(void)
{
	return m_source->name();
}

/**
 * Update the sprite buffers if the object is different
 */
bool dfObject::updateSprite(glm::vec3* position, glm::vec3* texture)
{
	glm::vec3 level(m_x, m_y, m_z);
	glm::vec3 gl;
	dfLevel::level2gl(level, gl);

	position->x = gl.x;
	position->y = gl.y;
	position->z = gl.z;

	texture->r = (float)m_source->id();
	texture->g = (float)m_state;
	texture->b = (float)m_frame;
	return true;
}

/**
 * Animate the object frame
 */
bool dfObject::update(time_t t)
{
	if (m_logics & DF_LOGIC_ANIM){
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

/**
 * Stack up logics
 */
void dfObject::logic(int logic)
{
	m_logics |= logic;

	if (logic & DF_LOGIC_SCENERY) {
		m_state = DF_STATE_SCENERY_NORMAL;
	}
	else if (logic & DF_ENEMIES) {
		m_state = DF_STATE_ENEMY_STAY_STILL;
	}
}

dfObject::~dfObject()
{
}
