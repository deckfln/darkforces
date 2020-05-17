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
	m_pch = pch;
	m_yaw = yaw;
	m_rol = rol;
	m_difficulty = difficulty;
}

/**
 * Check the name of th associated WAX
 */
bool dfObject::named(std::string name)
{
	return m_source->named(name);
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

	texture->r = (float)m_source->textureID(m_state, m_frame);
	return true;
}

/**
 * Animate the object frame
 */
bool dfObject::update(time_t t)
{
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

	return false;
}

dfObject::~dfObject()
{
}
