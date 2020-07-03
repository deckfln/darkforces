#include "gaEntity.h"

#include "../gaEngine/gaBoundingBoxes.h"

static int g_ids = 0;

gaEntity::gaEntity(int mclass, const std::string& name) :
	m_entityID(g_ids++),
	m_name(name),
	m_class(mclass)
{
}

gaEntity::gaEntity(int mclass, const std::string& name, const glm::vec3& position):
	m_entityID(g_ids++),
	m_name(name),
	m_class(mclass),
	m_position(position)
{
}

/**
 * set the model space AABB
 */
void gaEntity::modelAABB(const fwAABBox& box)
{
	m_modelAABB = box;
}

/**
 * create a boundingbox mesh
 */
void gaEntity::drawBoundingBox(void)
{
	g_gaBoundingBoxes.add(&m_worldBounding);
}


/**
 * Update the object position (given in gl space) and update the worldboundingBox(in gl space)
 */
void gaEntity::moveTo(const glm::vec3& position)
{
	m_position = position;

	// take the opportunity to update the world bounding box
	updateWorldAABB();
}

/**
 * update the world AABB based on position
 */
void gaEntity::updateWorldAABB(void)
{
	m_worldBounding.translateFrom(m_modelAABB, m_position);
}

gaEntity::~gaEntity()
{
}
