#include "dfObject.h"

#include "dfModel.h"
#include "dfSprites.h"
#include "dfLevel.h"
#include "dfModel/df3DO.h"
#include "../framework/fwScene.h"

dfObject::dfObject(dfModel *source, glm::vec3& position, float ambient, int type):
	m_source(source),
	m_position(position),
	m_ambient(ambient),
	m_is(type)
{
}

/**
 * Check the name of th associated WAX
 */
bool dfObject::named(std::string name)
{
	return m_source->named(name);
}

/**
 * test the nature of the object
 */
bool dfObject::is(int type)
{
	return m_is == type;
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

void dfObject::add2scene(fwScene* scene)
{
	df3DO* model = (df3DO *)m_source;
	if (model) {
		glm::vec3 gl;
		dfLevel::level2gl(m_position, gl);

		model->add2scene(scene, gl);
	}
}

dfObject::~dfObject()
{
}
