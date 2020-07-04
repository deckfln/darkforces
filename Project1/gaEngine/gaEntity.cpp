#include "gaEntity.h"

#include "../darkforces/dfMessageBus.h"
#include "../gaEngine/gaBoundingBoxes.h"

static int g_ids = 0;

gaEntity::gaEntity(int mclass, const std::string& name) :
	m_entityID(g_ids++),
	m_name(name),
	m_class(mclass)
{
	g_gaBoundingBoxes.add(&m_worldBounding);
	g_MessageBus.addClient(this);

}

gaEntity::gaEntity(int mclass, const std::string& name, const glm::vec3& position):
	m_entityID(g_ids++),
	m_name(name),
	m_class(mclass),
	m_position(position)
{
	g_gaBoundingBoxes.add(&m_worldBounding);
	g_MessageBus.addClient(this);
}

/**
 * quick AABB check for entities collision
 */
bool gaEntity::collideAABB(fwAABBox& box)
{
	return m_worldBounding.intersect(box);
}

/**
 * set the model space AABB
 */
void gaEntity::modelAABB(const fwAABBox& box)
{
	m_modelAABB = box;
}

/**
 * Update the object position (given in gl space) and update the worldboundingBox(in gl space)
 */
void gaEntity::moveTo(const glm::vec3& position)
{
	if (m_name == "elev3-1_panel1(2)") {
		printf("gaEntity::moveTo");
	}
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
