#include "dfObject.h"

#include "dfModel.h"
#include "dfSprites.h"
#include "dfLevel.h"

dfObject::dfObject(dfModel *source, glm::vec3& position, float ambient, int type):
	m_source(source),
	m_position_lvl(position),
	m_ambient(ambient),
	m_is(type)
{
	update(position);
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

/**
 * test the logic
 */
bool dfObject::isLogic(int logic)
{
	return (m_logics & logic) != 0;
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
 * Update the object position (given in level space) and update the worldboundingBox(in gl space)
 */
void dfObject::update(const glm::vec3& position)
{
	m_position_lvl = position;
	dfLevel::level2gl(m_position_lvl, m_position_gl);

	// take the opportunity to update the world bounding box
	m_worldBounding.translateFrom(m_source->bounding(), m_position_gl);

	if (m_meshAABB) {
		// and update the gl boundingbox
		m_meshAABB->translate(m_position_gl);
	}

	m_dirtyPosition = true;
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

/**
 * create a boundingbox mesh
 */
fwMesh* dfObject::drawBoundingBox(void)
{
	m_meshAABB = m_source->drawBoundingBox()->clone();
	m_meshAABB->translate(m_position_gl);

	return m_meshAABB;
}

dfObject::~dfObject()
{
}
