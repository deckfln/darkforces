#include "gaEntity.h"

#include "../framework/fwScene.h"
#include "../framework/fwMesh.h"

#include "gaWorld.h"
#include "../gaEngine/gaBoundingBoxes.h"

static int g_ids = 0;

gaEntity::gaEntity(int mclass, const std::string& name) :
	m_entityID(g_ids++),
	m_name(name),
	m_class(mclass)
{
	g_gaBoundingBoxes.add(&m_worldBounding);
	g_gaWorld.addClient(this);
}

gaEntity::gaEntity(int mclass, const std::string& name, const glm::vec3& position):
	m_entityID(g_ids++),
	m_name(name),
	m_class(mclass),
	m_position(position)
{
	g_gaBoundingBoxes.add(&m_worldBounding);
	g_gaWorld.addClient(this);
}

/**
 * add an entity inside that one (and increase the AABB if needed)
 */
void gaEntity::addChild(gaEntity* entity)
{
	m_children.push_back(entity);
	m_modelAABB.extend(entity->m_modelAABB);
	updateWorldAABB();
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
 * rotate the object and update the AABB
 */
void gaEntity::rotate(const glm::vec3& rotation)
{
	m_rotation = rotation;
	m_quaternion = glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));

	// take the opportunity to update the world bounding box
	updateWorldAABB();
}

/**
 * distance between the 2 entities
 */
float gaEntity::distanceTo(gaEntity* other)
{
	return glm::distance(m_position, other->m_position);
}

/**
 * if the entity has a mesh, add to the scene
 */
void gaEntity::add2scene(fwScene* scene)
{
	if (m_mesh) {
		m_scene = scene;
		m_scene->addChild(m_mesh);
	}
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
	m_worldBounding.rotateFrom(m_modelAABB, m_quaternion);
	m_worldBounding += m_position;
}

/**
 * extended collision test after a succefull AABB collision
 */
bool gaEntity::checkCollision(fwCylinder& bounding, glm::vec3& direction, glm::vec3& intersection, std::list<gaCollisionPoint>& collisions)
{
	// default entities are included in the AABB, so the test is always positive
	return true;
}

gaEntity::~gaEntity()
{
	g_gaWorld.removeClient(this);
	g_gaBoundingBoxes.remove(&m_worldBounding);

	if (m_mesh) {
		if (m_scene) {
			m_scene->removeChild(m_mesh);
		}
		delete m_mesh;
	}
}