#include "dfObject.h"

#include "dfModel.h"
#include "dfSprites.h"
#include "dfLevel.h"

dfObject::dfObject(dfModel *source, glm::vec3& position, float ambient):
	m_source(source),
	m_position(position),
	m_ambient(ambient)
{
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
bool dfObject::updateSprite(glm::vec3* position, glm::vec4* texture, glm::vec3* direction)
{
	glm::vec3 gl;
	dfLevel::level2gl(m_position, gl);

	*position = gl;

	texture->r = (float)m_source->id();
	texture->a = m_ambient;
	return true;
}

/**
 * Animate the object frame
 */
bool dfObject::update(time_t t)
{
	return false;
}

/**
 * Stack up logics
 */
void dfObject::logic(int logic)
{
	m_logics |= logic;
}

dfObject::~dfObject()
{
}
